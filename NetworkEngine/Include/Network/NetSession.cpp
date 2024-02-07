#include "NetSession.h"

CNetSession::CNetSession()
{
	memset(&m_tAcceptOverlapped, 0, sizeof(m_tAcceptOverlapped));
	memset(&m_tReadOverlapped, 0, sizeof(m_tReadOverlapped));
	memset(&m_tWriteOverlapped, 0, sizeof(m_tWriteOverlapped));
	memset(m_ReadBuffer, 0, MAX_BUFFER_LENGTH);
	memset(&m_UDPRemoteAddr, 0, sizeof(m_UDPRemoteAddr));

	m_hSocket = 0;

	m_hUDPThreadHandle = 0;
	m_hUDPThreadStartupEvent = 0;
	m_hUDPThreadDestroyEvent = 0;
	m_hUDPThreadWakeUpEvent = 0;
	m_hUDPThreadCompleteEvent = 0;

	m_tAcceptOverlapped.eIOType = IO_ACCEPT;
	m_tReadOverlapped.eIOType = IO_READ;
	m_tWriteOverlapped.eIOType = IO_WRITE;

	m_tAcceptOverlapped.pObject = this;
	m_tReadOverlapped.pObject = this;
	m_tWriteOverlapped.pObject = this;

	m_bUdpSending = false;

	m_iUserNumber = -1;
}

CNetSession::~CNetSession()
{
}

bool CNetSession::Begin()
{
	CThreadSync	Sync;

	if (m_hSocket)
		return false;

	memset(m_ReadBuffer, 0, MAX_BUFFER_LENGTH);
	memset(&m_UDPRemoteAddr, 0, sizeof(m_UDPRemoteAddr));

	m_hSocket = 0;
	m_hUDPThreadHandle = 0;
	m_hUDPThreadStartupEvent = 0;
	m_hUDPThreadDestroyEvent = 0;
	m_hUDPThreadWakeUpEvent = 0;
	m_hUDPThreadCompleteEvent = 0;

	m_bUdpSending = false;

	return true;
}

bool CNetSession::End()
{
	CThreadSync	Sync;

	if (!m_hSocket)
		return false;

	closesocket(m_hSocket);
	m_hSocket = 0;

	if (m_hUDPThreadHandle)
	{
		SetEvent(m_hUDPThreadDestroyEvent);

		WaitForSingleObject(m_hUDPThreadHandle, INFINITE);

		CloseHandle(m_hUDPThreadHandle);
	}

	if (m_hUDPThreadStartupEvent)
		CloseHandle(m_hUDPThreadStartupEvent);

	if (m_hUDPThreadDestroyEvent)
		CloseHandle(m_hUDPThreadDestroyEvent);

	if (m_hUDPThreadWakeUpEvent)
		CloseHandle(m_hUDPThreadWakeUpEvent);

	if (m_hUDPThreadCompleteEvent)
		CloseHandle(m_hUDPThreadCompleteEvent);

	return true;
}

bool CNetSession::Listen(USHORT sPort, int iBackLog)
{
	// iBackLog : �󸶳� �����Ұ��� Ŭ��
	CThreadSync	Sync;

	if (sPort <= 0 || iBackLog <= 0)
		return false;

	if (!m_hSocket)
		return false;

	SOCKADDR_IN	tAddr = {};

	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(sPort);
	tAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_hSocket, (SOCKADDR*)&tAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		End();
		return false;
	}

	if (listen(m_hSocket, iBackLog) == SOCKET_ERROR)
	{
		End();
		return false;
	}

	// SO_LINGER : �� �ɼ��� ������ ������ ���۵��� ���� �����͸� ��� ó�������� ������ �� �ִ�.
	LINGER	tLinger = {};
	tLinger.l_onoff = 1; // Ȱ��ȭ
	tLinger.l_linger = 0; // Ȱ��ȭ�� ���ð�, 0�Ͻ� ���Ϲ��ۻ���

	/*
	1) ling.l_onoff = 1, ling.l_linger = 0
	closesocket() �Լ��� ��� ��ȯ, ���ۿ� ���� �����ʹ� ����.
	���� ����(abortive shutdown)ó��. TIME_WAIT�� ���� ����.
	2) ling.l_onoff = 1, ling.l_linger != 0
	blocking ����� ���� ����(graceful shutdown)ó��
	3) ling.l_onoff = 0
	non-blocking ����� ������ ����ó��. ����ó���� �ϰ�� ������ �˼��� ����.
	*/

	if (setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (char*)&tLinger, sizeof(LINGER)) == SOCKET_ERROR)
	{
		End();
		return false;
	}
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&tLinger, sizeof(LINGER)) == SOCKET_ERROR)
	{
		End();
		return false;
	}
	if (setsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&tLinger, sizeof(LINGER)) == SOCKET_ERROR)
	{
		End();
		return false;
	}

	// GetSockOpt ���� �ɼ������� �°� ���� ��� ������ �Ǿ��ִ����� �˾Ƴ��� �Լ�
	// ��ü �ɼǵ��� ������ �������°��� �ƴ� Ư�� �ɼǸ��� ���� �Լ�ȣ���� ���� ã�Ƴ��� �Ѵ�.

	//int OptionName = 0;
	//LINGER	stLinger = {};
	//int OptionLen = sizeof(LINGER);

	//if (getsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, &OptionLen) == SOCKET_ERROR)
	//{

	//}

	//if (getsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&stLinger, &OptionLen) == SOCKET_ERROR)
	//{

	//}

	//if (getsockopt(m_hSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&stLinger, &OptionLen) == SOCKET_ERROR)
	//{

	//}

	return true;
}

bool CNetSession::Accept(SOCKET hListenSocket) // �Ű����� 
{
	CThreadSync	Sync;

	if (!hListenSocket)
		return false;

	if (m_hSocket)
		return false;
	// Ŭ����� �̸� �������� ���� ����� Ŭ������� ����.
	m_hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	if (m_hSocket == INVALID_SOCKET)
	{   
		End();
		return false;
	}
	if (!AcceptEx(hListenSocket, m_hSocket, m_ReadBuffer, 0, sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16, nullptr, &m_tAcceptOverlapped.tOverlapped))
	{ //ERROR_IO_PENDING �񵿱� ó���Ϸ� / WSAEWOULDBLOCK ���� �񵿱� �۾���.. ���� ���� ��û�� ���� ����
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();
			return false;
		}
	}

	return true;
}

bool CNetSession::TcpBind()
{
	CThreadSync	Sync;

	if (m_hSocket)
		return false;

	m_hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	if (m_hSocket == INVALID_SOCKET)
	{
		End();
		return false;
	}

	return true;
}

bool CNetSession::UdpBind()
{
	return false;
}

bool CNetSession::GetLocalIP(TCHAR* pIP) const
{
	CThreadSync	sync;

	if (!m_hSocket)
		return false;

	char	strName[256] = {};
	// getsockname, getpeername, getaddrinfo, gethostname, gethostbyname ����
	gethostname(strName, sizeof(strName));

	PHOSTENT	pHost = gethostbyname(strName);
	/*
		struct hostent {
		char *h_name; //���� ������ �̸�,
		char **h_aliases; //���� �̿� ������ �̸���
		int h_addrtype; //�ּ����� ü��
		(IPv4: AF_INET, IPv6: AF_INET6) int h_length; //IP�ּ��� ũ�⸦ ��´�.
		(IPv4�� 4) char **h_addr_list; //������ �̸��� ���� IP�ּҰ� ���� ���·� ��ȯ�� �� �� ��� ������ �̿�
		}

		-MultiByteToWideChar-
		1.CodePage"�� ANSII ���ڿ��� � ����ΰ��� �����ϴ� �ŰԺ����̴�. / CP_ACP"�� OS�� �⺻ ���� ������ ��� �ڵ带 ����Ͽ� ��ȯ�Ѵٴ� ���̴�.
		2.dwFlags"�� �Ϲ������δ� ������ ������ �������� ������ MSDN�� ���캸�� �ٶ���.
		3.lpMultiByteStr�� UNICODE�� ������ ���ϴ� ANSII ���ڿ��̴�.
		4.cbMultiByte���� �����ϰ��� �ϴ� ANSII ���ڿ��� ���� �̴�. "-1"�� ������ ���̸� �� �Լ��� ����Ѵ�.
		5.lpWideCharStr���� UNICODE�� ����� ���ڿ��� �� �����̴�.
		6.cchWideChar���� ����� UNICODE ���ڿ��� �����̴�.
		�� ���������� MultiByteToWideChar() �Լ��� ȣ��Ǵµ� �̶� �ڵ��������� ��CP_ACP���� �����ǹǷ�
		�̸� �����Ͽ��� �Ѵ�. ���� ������ ������ ��Regional Setting���� ����� �Ǿ� ���� ������ �ѱ� ��ȯ�� ���� ���� ���̴�.
		CString �� ANSII �ڵ带 UNICODE�� �������ִ� ���� �����ڰ� ������� �����Ƿ� �ܼ� �������� ��ȯ�� �Ͼ��.


	*/
	if (pHost)
	{
		// inet_ntoa(*(in_addr*)pHost->h_addr_list)) �� �ּ� ��Ͽ��� in_addr�� char* �� ���´�.

		if (MultiByteToWideChar(CP_ACP, 0, inet_ntoa(*(in_addr*)pHost->h_addr_list), -1, pIP, 32) > 0)
			return true;
	}

	return false;
}

unsigned short CNetSession::GetLocalPort() const
{
	CThreadSync	sync;

	if (!m_hSocket)
		return false;

	SOCKADDR_IN	tAddr = {};

	int	iAddrLength = sizeof(tAddr);

	if (getsockname(m_hSocket, (sockaddr*)&tAddr, &iAddrLength) != SOCKET_ERROR)
		return ntohs(tAddr.sin_port);

	return 0;
}

bool CNetSession::InitializeReadForIocp()
{
	CThreadSync		sync;

	if (!m_hSocket)
		return false;

	WSABUF	wsabuf;
	DWORD	dwRecvBytes = 0;
	DWORD	dwRecvFlag = 0;

	wsabuf.buf = (char*)m_ReadBuffer;
	wsabuf.len = MAX_BUFFER_LENGTH;

	int	iReturnValue = WSARecv(m_hSocket, &wsabuf, 1, &dwRecvBytes, &dwRecvFlag, &m_tReadOverlapped.tOverlapped,
		nullptr);

	if (iReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return false;
	}

	return true;
}

bool CNetSession::ReadForIocp(BYTE* pData, unsigned int& iLength)
{
	CThreadSync		sync;

	if (!m_hSocket || !pData || iLength <= 0)
		return false;

	memcpy(pData, m_ReadBuffer, iLength);

	return true;
}

bool CNetSession::ReadForEventSelect(BYTE* pData, DWORD& dwLength)
{
	CThreadSync		sync;

	if (!m_hSocket || !pData)
		return false;

	WSABUF	wsabuf;
	DWORD	dwRecvBytes = 0;
	DWORD	dwRecvFlag = 0;

	wsabuf.buf = (char*)m_ReadBuffer;
	wsabuf.len = MAX_BUFFER_LENGTH;

	int	iReturnValue = WSARecv(m_hSocket, &wsabuf, 1, &dwRecvBytes, &dwRecvFlag, &m_tReadOverlapped.tOverlapped,
		nullptr);

	if (iReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return false;
	}

	memcpy(pData, m_ReadBuffer, dwRecvBytes);
	dwLength = dwRecvBytes;

	return true;
}

bool CNetSession::Write(BYTE* pData, DWORD dwLength)
{
	CThreadSync		sync;

	if (!m_hSocket || !pData || dwLength <= 0)
		return false;

	WSABUF	wsabuf;
	DWORD	dwSendBytes = 0;
	DWORD	dwSendFlag = 0;

	wsabuf.buf = (char*)pData;
	wsabuf.len = dwLength;

	int	iReturnValue = WSASend(m_hSocket, &wsabuf, 1, &dwSendBytes, dwSendFlag, &m_tWriteOverlapped.tOverlapped,
		nullptr);

	if (iReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return false;
	}

	return true;
}

bool CNetSession::Connect(const char* pAddr, USHORT sPort)
{
	CThreadSync	sync;
	if (!m_hSocket || !pAddr || sPort <= 0)
		return false;

	SOCKADDR_IN	tAddr = {};

	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(sPort);
	tAddr.sin_addr.s_addr = inet_addr(pAddr);

	if (WSAConnect(m_hSocket, (sockaddr*)&tAddr, sizeof(SOCKADDR_IN), nullptr, nullptr, nullptr, nullptr) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();
			return false;
		}
	}

	return true;
}
