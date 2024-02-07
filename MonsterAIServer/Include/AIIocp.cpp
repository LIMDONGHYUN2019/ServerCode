#include "AIIocp.h"
#include "Network/PacketSession.h"
#include "Network/NetSession.h"
#include "AIServerManager.h"
#include "Log.h"
#include "Stream.h"
#include <iostream>
#include "PathManager.h"
#include "CircleQueue.h"
#include "Navigation/NavigationManager.h"
#include "AISessionManager.h"

using namespace std;

CAIIocp::CAIIocp()
{
}

CAIIocp::~CAIIocp()
{
}

bool CAIIocp::Begin()
{
	if (!CIocp::Begin())
		return false;

	if (!RegisterSocketToIocp(m_pListenSession->GetSocket(), (ULONG_PTR)m_pListenSession))
	{
		CAIIocp::End();
		return false;
	}

	if(!CAISessionManager::GetInst()->Begin(m_pListenSession->GetSocket()))
	{
		CAIIocp::End();
		return false;
	}

	return true;
}

bool CAIIocp::End()
{
	if (!CIocp::End())
		return false;

	return true;
}

void CAIIocp::OnIoRead(void* pObj, unsigned int iLength)
{
	CPacketSession* pSession = (CPacketSession*)pObj;

	if (pSession->ReadPacketForIocp(iLength))
	{
		unsigned int	iProtocol;
		unsigned int	iPacketLength = 0;
		BYTE		packet[MAX_BUFFER_LENGTH] = {};

		while (pSession->GetPacket(iProtocol, packet, iPacketLength))
		{
			//CCircleQueue<PacketQueueData, 1000>* pQueue = CGameManager::GetInst()->GetPacketQueue();

			//PacketQueueData	Data;
			//Data.Protocol = iProtocol;
			//Data.Length = iPacketLength;
			//Data.pSession = pSession;
			//memcpy(Data.Packet, packet, iPacketLength);

			//pQueue->push(Data);


			switch (iProtocol)
			{
			case AP_FINDPATH:
				FindPath(packet, pSession);
				break;
			}
		}
	}

	pSession->InitializeReadForIocp();
}

void CAIIocp::OnIoWrite(void* pObj, unsigned int iLength)
{
}

void CAIIocp::OnIoConnected(void* pObj)
{
	CPacketSession* pSession = (CPacketSession*)pObj;

	cout << pSession->GetSocket() << " Client Connect" << endl;

	// 접속한 유저를 Iocp 에 등록한다.
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		End();
		return;
	}

	// 세션의 Iocp 초기화
	pSession->InitializeReadForIocp();
}

void CAIIocp::OnIoDisconnected(void* pObj)
{
	CPacketSession* pSession = (CPacketSession*)pObj;

	cout << pSession->GetSocket() << " Client Disconnect" << endl;

	pSession->End();

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

void CAIIocp::FindPath(BYTE* pPacket, CPacketSession* pSession)
{
	// 실제 길을 찾을때는 받는 패킷
	// 내비게이션이름/몬스터번호/시작위치/도착위치/ 4가지를 받아서 결과로 길을 찾은 경로를 해당 몬스터로
	// 보내주도록 한다.
	CStream	stream;

	stream.SetBuffer((char*)pPacket);

	char	NavName[32] = {};
	int		NameLength = 0;
	int		MonsterNumber = 0;
	Vector3	StartPos;
	Vector3	EndPos;

	stream.Read(&NameLength, 4);
	stream.Read(NavName, NameLength);
	stream.Read(&MonsterNumber, 4);
	ELevel	LevelType;
	stream.Read(&LevelType, sizeof(ELevel));
	stream.Read(&StartPos, 12);
	stream.Read(&EndPos, 12);

	CNavigationManager::GetInst()->FindPath(pSession, NavName, MonsterNumber, LevelType, StartPos, EndPos);
}

