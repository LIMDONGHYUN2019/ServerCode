#pragma once
#include "Thread.h"
#include "GameInfo.h"
#include "Network/PacketSession.h"
#include "Stream.h"

class AIThread :
    public CThread
{
public:
	AIThread();
	~AIThread();
private:
	SOCKET m_hSocket;
	sockaddr_in m_taddr = {};
	int m_iAddrSize;

	class CPacketSession* PacketSession;

public:
	bool Init();
	void Run();
    
};

