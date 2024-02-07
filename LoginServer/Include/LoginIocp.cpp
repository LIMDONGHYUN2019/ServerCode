#include "LoginIocp.h"
#include "Network/NetSession.h"
#include "UserManager.h"
#include "UserSession.h"
#include "Log.h"
#include "Stream.h"
#include <iostream>
#include "DatabaseManager.h"

using namespace std;

CLoginIocp::CLoginIocp()
{
}

CLoginIocp::~CLoginIocp()
{
}

bool CLoginIocp::Begin()
{
	if (!CIocp::Begin())
		return false;

	if (!RegisterSocketToIocp(m_pListenSession->GetSocket(), (ULONG_PTR)m_pListenSession))
	{
		CLoginIocp::End();
		return false;
	}

	// ���� �����ڸ� �����Ѵ�.
	m_pUserManager = GET_SINGLE(CUserManager);

	if (!m_pUserManager->Begin(m_pListenSession->GetSocket()))
	{
		CLoginIocp::End();
		return false;
	}

	return true;
}

bool CLoginIocp::End()
{
	if (!CIocp::End())
		return false;

	return true;
}

void CLoginIocp::OnIoRead(void* pObj, unsigned int iLength)
{
	cout << "Client Write" << endl;

	CUserSession* pSession = (CUserSession*)pObj;

	if (pSession->ReadPacketForIocp(iLength))
	{
		unsigned int	iProtocol;
		unsigned int	iPacketLength = 0;
		BYTE		packet[MAX_BUFFER_LENGTH] = {};

		while (pSession->GetPacket(iProtocol, packet, iPacketLength))
		{
			switch (iProtocol)
			{
			case LP_MEMBERSHIP:
				Membership(packet, pSession);
				break;

			case LP_LOGIN:
				Login(packet, pSession);
				break;

			case LP_LOGOUT:
				break;
			}
		}
	}

	pSession->InitializeReadForIocp();
}

void CLoginIocp::OnIoWrite(void* pObj, unsigned int iLength)
{
}

void CLoginIocp::OnIoConnected(void* pObj)
{
	CUserSession* pSession = (CUserSession*)pObj;

	cout << pSession->GetSocket() << " Client Connect" << endl;

	// ������ ������ Iocp �� ����Ѵ�.
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		//CLog::WriteLog(TEXT("Register Iocp Failed!!"));
		End();
		return;
	}

	// ������ Iocp �ʱ�ȭ
	pSession->InitializeReadForIocp();
}

void CLoginIocp::OnIoDisconnected(void* pObj)
{
	CUserSession* pSession = (CUserSession*)pObj;

	cout << pSession->GetSocket() << " Client Disconnect" << endl;

	pSession->End();

	if (!pSession->Begin())
	{
		pSession->End();
		return;
	}

	// �񵿱� Accept ���·� ������־ ���ӵǴ¼��� �� ���ǿ� ������ �Ҵ����ش�.
	if (!pSession->Accept(m_pListenSession->GetSocket()))
	{
		pSession->End();
		return;
	}
}

void CLoginIocp::Membership(BYTE* pPacket, class CUserSession* pSession)
{
	int	iLength = 0;

	CStream	stream;
	stream.SetBuffer((char*)pPacket);
	
	// �̸�, Email, ID, Password ������ ���´�.
	TCHAR	_strName[ID_SIZE] = {};
	TCHAR	_strEmail[PASS_SIZE] = {};
	TCHAR	_strID[ID_SIZE] = {};
	TCHAR	_strPass[PASS_SIZE] = {};

	char	strName[ID_SIZE] = {};
	char	strEmail[PASS_SIZE] = {};
	char	strID[ID_SIZE] = {};
	char	strPass[PASS_SIZE] = {};

	stream.Read<int>(&iLength, sizeof(int));
	stream.Read((char*)_strName, iLength);

	stream.Read<int>(&iLength, sizeof(int));
	stream.Read((char*)_strEmail, iLength);

	stream.Read<int>(&iLength, sizeof(int));
	stream.Read((char*)_strID, iLength);

	stream.Read<int>(&iLength, sizeof(int));
	stream.Read((char*)_strPass, iLength);

	int iCount = WideCharToMultiByte(CP_ACP, 0, _strName, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strName, -1, strName, iCount, 0, 0);

	iCount = WideCharToMultiByte(CP_ACP, 0, _strEmail, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strEmail, -1, strEmail, iCount, 0, 0);

	iCount = WideCharToMultiByte(CP_ACP, 0, _strID, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strID, -1, strID, iCount, 0, 0);

	iCount = WideCharToMultiByte(CP_ACP, 0, _strPass, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strPass, -1, strPass, iCount, 0, 0);

	if (GET_SINGLE(CDatabaseManager)->SignUp(strName, strEmail, strID, strPass))
	{
		cout << "ȸ������ ����" << endl;

		// ��� Ŭ���̾�Ʈ���� ���� ������ IP �ּҸ� �������ش�.
		char	packet[MAX_BUFFER_LENGTH] = {};

		CStream	stream;
		stream.SetBuffer(packet);

		int	iLength = strlen(strID);
		stream.Write<int>(&iLength, sizeof(int));
		stream.Write(strID, iLength);

		iLength = strlen(GAMESERVER_IP);
		stream.Write(&iLength, sizeof(int));
		stream.Write(GAMESERVER_IP, iLength);

		short	sPort = GAMESERVER_PORT;
		stream.Write(&sPort, sizeof(short));

		pSession->WritePacket(LP_MEMBERSHIP_SUCCESS, (BYTE*)packet, stream.GetLength());

		// �� ������ ���̻� �ʿ� �����Ƿ� �����Ѵ�.

		return;
	}

	else
	{
		cout << "ȸ������ ����" << endl;

		char	packet[MAX_BUFFER_LENGTH] = {};

		pSession->WritePacket(LP_MEMBERSHIP_FAIL, (BYTE*)packet, stream.GetLength());
	}
}

void CLoginIocp::Login(BYTE* pPacket, class CUserSession* pSession)
{
	int	iLength = 0;

	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	char	strID[128] = {};
	char	strPass[128] = {};

	TCHAR	_strID[128] = {};
	TCHAR	_strPass[128] = {};

	stream.Read<int>(&iLength, sizeof(int));
	stream.Read((char*)_strID, iLength);

	stream.Read<int>(&iLength, sizeof(int));
	stream.Read((char*)_strPass, iLength);

	// Multibyte�� �ٲ۴�.
	int iCount = WideCharToMultiByte(CP_ACP, 0, _strID, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strID, -1, strID, iCount, 0, 0);

	iCount = WideCharToMultiByte(CP_ACP, 0, _strPass, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strPass, -1, strPass, iCount, 0, 0);

	int	iUserNumber = -1;

	if (GET_SINGLE(CDatabaseManager)->Login(&iUserNumber, strID, strPass))
	{
		cout << "Login ����" << endl;

		pSession->SetUserNumber(iUserNumber);

		// ��� Ŭ���̾�Ʈ���� ���� ������ IP �ּҸ� �������ش�.
		char	packet[MAX_BUFFER_LENGTH] = {};

		CStream	stream;
		stream.SetBuffer(packet);

		// ������ȣ�� �־��ش�.
		stream.Write<int>(&iUserNumber, sizeof(int));

		int	iLength = lstrlen(_strID);
		stream.Write<int>(&iLength, sizeof(int));
		stream.Write(_strID, iLength * sizeof(TCHAR));

		TCHAR	strIP[128] = {};

		iCount = MultiByteToWideChar(CP_ACP, 0, GAMESERVER_IP, -1, 0, 0);
		MultiByteToWideChar(CP_ACP, 0, GAMESERVER_IP, -1, strIP, iCount);

		iLength = strlen(GAMESERVER_IP);
		stream.Write(&iLength, sizeof(int));
		stream.Write(strIP, iLength * sizeof(TCHAR));

		short	sPort = GAMESERVER_PORT;
		stream.Write(&sPort, sizeof(short));

		pSession->WritePacket(LP_LOGIN_SUCCESS, (BYTE*)packet, stream.GetLength());

		// �� ������ ���̻� �ʿ� �����Ƿ� �����Ѵ�.

		return;
	}

	else
	{
		cout << "Login ����" << endl;

		char	packet[MAX_BUFFER_LENGTH] = {};

		pSession->WritePacket(LP_LOGIN_FAIL, (BYTE*)packet, stream.GetLength());
	}
}
