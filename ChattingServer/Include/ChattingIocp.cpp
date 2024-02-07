#include "ChattingIocp.h"
#include "Network/NetSession.h"
#include "Log.h"
#include "Stream.h"
#include "UserManager.h"
#include "User.h"
#include <iostream>

using namespace std;

CChattingIocp::CChattingIocp()
{
	m_pListenSession = nullptr;
	m_pUserManager = nullptr;
}

CChattingIocp::~CChattingIocp()
{
}

bool CChattingIocp::Begin()
{
	if (!CIocp::Begin())
		return false;

	if (!RegisterSocketToIocp(m_pListenSession->GetSocket(), (ULONG_PTR)m_pListenSession))
	{
		CChattingIocp::End();
		return false;
	}

	// 유저 관리자를 생성한다.
	m_pUserManager = GET_SINGLE(CUserManager);

	if (!m_pUserManager->Begin(m_pListenSession->GetSocket()))
	{
		CChattingIocp::End();
		return false;
	}

	return true;
}

bool CChattingIocp::End()
{
	if (!CIocp::End())
		return false;

	return true;
}

void CChattingIocp::OnIoRead(void* pObj, unsigned int iLength) 
{
	CUser* pSession = (CUser*)pObj;
	if (pSession->ReadPacketForIocp(iLength))
	{
		unsigned int	iProtocol;
		unsigned int	iPacketLength = 0;
		BYTE		packet[MAX_BUFFER_LENGTH] = {};

		while (pSession->GetPacket(iProtocol, packet, iPacketLength))
		{
			switch (iProtocol)
			{
			case CP_LOGIN:
				Login(packet, pSession);
				break;
			case CP_LOGOUT:
				break;
			case CP_MESSAGE:
				Message(packet,pSession);
				break;
			}
		}
	}

	pSession->InitializeReadForIocp();
}

void CChattingIocp::Message(BYTE* pPacket, CUser* pSession)
{
	char	ChattingMessage[MESSAGE_LENGTH] = {};
	int		ChattingLength = 0;

	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	stream.Read<int>(&ChattingLength, sizeof(int));
	stream.Read((char*)ChattingMessage, ChattingLength);

	TCHAR	wStrMessage[MESSAGE_LENGTH] = {};
	char strMessage[MESSAGE_LENGTH] = {};

	int	iLength = MultiByteToWideChar(CP_UTF8, 0, ChattingMessage, -1, 0, 0);
	if (iLength > 0)
	{
		MultiByteToWideChar(CP_UTF8, 0, ChattingMessage, -1, wStrMessage, iLength);
		int iWLength = WideCharToMultiByte(CP_ACP, 0, wStrMessage, iLength, 0, 0, 0, 0);
		if (iWLength > 0)
		{
			WideCharToMultiByte(CP_ACP, 0, wStrMessage, iLength, strMessage, iWLength, 0, 0);
		}
	}

	cout << pSession->GetCharacterName() << " : " << strMessage << endl;

	GET_SINGLE(CUserManager)->OtherUserSendChatting(pSession, ChattingMessage);
}

void CChattingIocp::Login(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);
	int iUserNumber = 0;
	char UserName[USERNAME_SIZE] = {};
	stream.Read<int>(&iUserNumber, sizeof(int));
	stream.Read(UserName, USERNAME_SIZE);

	pSession->SetUserNumber(iUserNumber);
	pSession->SetCharacterName(UserName);

	cout << "[ " << pSession->GetSocket() << " Client // ";
	cout << "User : " << pSession->GetCharacterName() << " Connect Complete ]" << endl;
}

void CChattingIocp::OnIoWrite(void* pObj, unsigned int iLength)
{
}

void CChattingIocp::OnIoConnected(void* pObj)
{
	CUser* pSession = (CUser*)pObj;
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		End();
		return;
	}

	pSession->SetConnect(true);
	GET_SINGLE(CUserManager)->AddConnectUser(pSession);
	GET_SINGLE(CUserManager)->ComformUserCount();
	pSession->InitializeReadForIocp();
}

void CChattingIocp::OnIoDisconnected(void* pObj)
{
	CUser* pSession = (CUser*)pObj;
	cout << pSession->GetSocket() << " Client Disconnect" << endl;
	pSession->End();
	if (!pSession->Begin())
	{
		pSession->End();
		return;
	}
	GET_SINGLE(CUserManager)->DisConnectUser(pSession);
	GET_SINGLE(CUserManager)->ComformUserCount();
	// 비동기 Accept 상태로 만들어주어서 접속되는순간 이 세션에 소켓을 할당해준다.
	if (!pSession->Accept(m_pListenSession->GetSocket()))
	{
		pSession->End();
		return;
	}
}
