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

	// ���� �����ڸ� �����Ѵ�.
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

	// ������ ������ Iocp �� ����Ѵ�.
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		End();

		return;
	}

	pSession->SetConnect(true);

	GET_SINGLE(CUserManager)->AddConnectUser(pSession);
	cout << "������ �����߽��ϴ�." << endl;
	// ������ Iocp �ʱ�ȭ
	pSession->InitializeReadForIocp();
}

void CGameIocp::OnIoDisconnected(void* pObj)
{
	CUser* pSession = (CUser*)pObj;

	cout << pSession->GetSocket() << " Client Disconnect" << endl;

	pSession->SaveInventory(); // ��������Ʈ�� ������ ����Ǹ� �׋� �Ҹ��ڸ� ȣ���� ���ִ� ������ ������������� ����.

	pSession->SetConnect(false);

	CLevel* Level = CLevelManager::GetInst()->FindLevel(pSession->GetLevelType());
	if (Level)
	{
		Level->DeleteUser(pSession);
	}

	pSession->End();

	GET_SINGLE(CUserManager)->DeleteConnectUser(pSession); //��ü ���� �����ϴ� �κ� 

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
