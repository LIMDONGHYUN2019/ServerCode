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
	// iBackLog : 얼마나 수용할건지 클라를
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

	// SO_LINGER : 이 옵션은 소켓을 닫을때 전송되지 않은 데이터를 어떻게 처리할지를 조정할 수 있다.
	LINGER	tLinger = {};
	tLinger.l_onoff = 1; // 활성화
	tLinger.l_linger = 0; // 활성화시 대기시간, 0일시 소켓버퍼삭제

	/*
	1) ling.l_onoff = 1, ling.l_linger = 0
	closesocket() 함수는 즉시 반환, 버퍼에 남은 데이터는 버림.
	강제 종료(abortive shutdown)처리. TIME_WAIT가 남지 않음.
	2) ling.l_onoff = 1, ling.l_linger != 0
	blocking 방식의 정상 종료(graceful shutdown)처리
	3) ling.l_onoff = 0
	non-blocking 방식의 정상적 종료처리. 종료처리가 완결된 시점을 알수는 없다.
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

	// GetSockOpt 현재 옵션종류에 맞게 현재 어떠한 설정이 되어있는지를 알아내는 함수
	// 전체 옵션들의 설정을 가져오는것이 아닌 특정 옵션마다 직접 함수호출을 통해 찾아내야 한다.

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

bool CNetSession::Accept(SOCKET hListenSocket) // 매개변수 
{
	CThreadSync	Sync;

	if (!hListenSocket)
		return false;

	if (m_hSocket)
		return false;
	// 클라소켓 미리 만들어놓은 소켓 여기로 클라소켓이 들어옴.
	m_hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	if (m_hSocket == INVALID_SOCKET)
	{   
		End();
		return false;
	}
	if (!AcceptEx(hListenSocket, m_hSocket, m_ReadBuffer, 0, sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16, nullptr, &m_tAcceptOverlapped.tOverlapped))
	{ //ERROR_IO_PENDING 비동기 처리완료 / WSAEWOULDBLOCK 아직 비동기 작업중.. 아직 연결 요청을 받지 못함
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
	// getsockname, getpeername, getaddrinfo, gethostname, gethostbyname 차이
	gethostname(strName, sizeof(strName));

	PHOSTENT	pHost = gethostbyname(strName);
	/*
		struct hostent {
		char *h_name; //공식 도메인 이름,
		char **h_aliases; //공식 이외 도메인 이름들
		int h_addrtype; //주소정보 체계
		(IPv4: AF_INET, IPv6: AF_INET6) int h_length; //IP주소의 크기를 담는다.
		(IPv4는 4) char **h_addr_list; //도메인 이름에 대한 IP주소가 정수 형태로 반환될 때 이 멤버 변수를 이용
		}

		-MultiByteToWideChar-
		1.CodePage"는 ANSII 문자열이 어떤 언어인가를 지정하는 매게변수이다. / CP_ACP"는 OS에 기본 언어로 설정된 언어 코드를 사용하여 변환한다는 것이다.
		2.dwFlags"는 일반적으로는 사용되지 않으며 새부적인 내용은 MSDN을 살펴보기 바란다.
		3.lpMultiByteStr” UNICODE로 변경을 원하는 ANSII 문자열이다.
		4.cbMultiByte”는 변경하고자 하는 ANSII 문자열의 길이 이다. "-1"을 넣으면 길이를 이 함수가 계산한다.
		5.lpWideCharStr”는 UNICODE로 변경된 문자열이 들어갈 버퍼이다.
		6.cchWideChar”는 변경된 UNICODE 문자열의 길이이다.
		단 내부적으로 MultiByteToWideChar() 함수가 호출되는데 이때 코드페이지가 “CP_ACP”로 설정되므로
		이를 유의하여야 한다. 만약 위에서 설명한 “Regional Setting”이 제대로 되어 있지 않으면 한글 변환이 되지 않을 것이다.
		CString 에 ANSII 코드를 UNICODE로 변경해주는 대입 연산자가 만들어져 있으므로 단순 대입으로 변환이 일어난다.


	*/
	if (pHost)
	{
		// inet_ntoa(*(in_addr*)pHost->h_addr_list)) 는 주소 목록에서 in_addr을 char* 로 얻어온다.

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
