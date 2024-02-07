#pragma once

#include "AIInfo.h"
#include "CircleQueue.h"

class CAIServerManager
{
private:
	class CAIIocp* m_Iocp;
	class CNetSession* m_pListen;
	class CPacketSession* m_GameSession;
	bool			m_Loop;

	CCircleQueue<PacketQueueData, 1000>	m_PacketQueue;


public:
	CCircleQueue<PacketQueueData, 1000>* GetPacketQueue()
	{
		return &m_PacketQueue;
	}

public:
	bool Init();
	void Run();

	DECLARE_SINGLE(CAIServerManager);
};

