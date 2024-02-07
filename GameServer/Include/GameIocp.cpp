#include "GameIocp.h"
#include "Network/NetSession.h"
#include "UserManager.h"
#include "User.h"
#include "Log.h"
#include "Stream.h"
#include <iostream>
#include "DatabaseManager.h"
#include "GameManager.h"
#include "GameObject/NPC/MonsterManager.h"
#include "PathManager.h"
#include "Inventory.h"
#include "GameManager.h"
#include "CircleQueue.h"
#include "Level/Level.h"
#include "Level/LevelManager.h"

using namespace std;

CGameIocp::CGameIocp()
{
	m_pListenSession = nullptr;
	m_pUserManager = nullptr;
}

CGameIocp::~CGameIocp()
{
}

bool CGameIocp::Begin()
{
	if (!CIocp::Begin())
		return false;

	if (!RegisterSocketToIocp(m_pListenSession->GetSocket(), (ULONG_PTR)m_pListenSession))
	{
		CGameIocp::End();
		return false;
	}

	// 유저 관리자를 생성한다.
	m_pUserManager = GET_SINGLE(CUserManager);

	if (!m_pUserManager->Begin(m_pListenSession->GetSocket()))
	{
		CGameIocp::End();
		return false;
	}
	       
	return true;
}

bool CGameIocp::End()
{
	if (!CIocp::End())
		return false;

	return true;
}

void CGameIocp::ConnectSession(CPacketSession* Session)
{
	CPacketSession* pSession = (CPacketSession*)Session;

	// 접속한 유저를 Iocp 에 등록한다.
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		//CLog::WriteLog(TEXT("Register Iocp Failed!!"));
		End();
		return;
	}

	// 세션의 Iocp 초기화
	pSession->InitializeReadForIocp();
}

void CGameIocp::OnIoRead(void* pObj, unsigned int iLength)
{
	CPacketSession* pSession = (CPacketSession*)pObj;

	if (pSession->ReadPacketForIocp(iLength))
	{
		unsigned int	iProtocol;
		unsigned int	iPacketLength = 0;
		BYTE		packet[MAX_BUFFER_LENGTH] = {};

		while (pSession->GetPacket(iProtocol, packet, iPacketLength))
		{
			CCircleQueue<PacketQueueData, 1000>* pQueue = CGameManager::GetInst()->GetPacketQueue();
			
			PacketQueueData	Data;
			Data.Protocol = iProtocol;
			Data.Length = iPacketLength;
			Data.pSession = (CUser*)pSession;
			memcpy(Data.Packet, packet, iPacketLength);

			pQueue->push(Data);
		}
	}

	pSession->InitializeReadForIocp();
}

void CGameIocp::OnIoWrite(void* pObj, unsigned int iLength)
{
}

void CGameIocp::OnIoConnected(void* pObj)
{
	CUser* pSession = (CUser*)pObj;

	cout << pSession->GetSocket() << " Client Connect" << endl;

	// 접속한 유저를 Iocp 에 등록한다.
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		End();

		return;
	}

	pSession->SetConnect(true);

	GET_SINGLE(CUserManager)->AddConnectUser(pSession);
	cout << "유저가 접속했습니다." << endl;
	// 세션의 Iocp 초기화
	pSession->InitializeReadForIocp();
}

void CGameIocp::OnIoDisconnected(void* pObj)
{
	CUser* pSession = (CUser*)pObj;

	cout << pSession->GetSocket() << " Client Disconnect" << endl;

	pSession->SaveInventory(); // 유저리스트가 서버가 종료되면 그떄 소멸자를 호출을 해주니 유저가 연결끊을떄마다 저장.

	pSession->SetConnect(false);

	CLevel* Level = CLevelManager::GetInst()->FindLevel(pSession->GetLevelType());
	if (Level)
	{
		Level->DeleteUser(pSession);
	}

	pSession->End();

	GET_SINGLE(CUserManager)->DeleteConnectUser(pSession); //전체 유저 관리하는 부분 

	if (!pSession->Begin())
	{
		pSession->End();
		return;
	}

	// 비동기 Accept 상태로 만들어주어서 접속되는순간 이 세션에 소켓을 할당해준다.
	if (!pSession->Accept(m_pListenSession->GetSocket()))
	{
		pSession->End();
		return;
	}
}
