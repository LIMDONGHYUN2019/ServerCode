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

	// ������ ������ Iocp �� ����Ѵ�.
	if (!RegisterSocketToIocp(pSession->GetSocket(), (ULONG_PTR)pSession))
	{
		End();
		return;
	}

	// ������ Iocp �ʱ�ȭ
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

	// �񵿱� Accept ���·� ������־ ���ӵǴ¼��� �� ���ǿ� ������ �Ҵ����ش�.
	if (!pSession->Accept(m_pListenSession->GetSocket()))
	{
		pSession->End();
		return;
	}
}

void CAIIocp::FindPath(BYTE* pPacket, CPacketSession* pSession)
{
	// ���� ���� ã������ �޴� ��Ŷ
	// ������̼��̸�/���͹�ȣ/������ġ/������ġ/ 4������ �޾Ƽ� ����� ���� ã�� ��θ� �ش� ���ͷ�
	// �����ֵ��� �Ѵ�.
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

