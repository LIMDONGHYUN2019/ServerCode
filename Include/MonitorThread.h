#pragma once
#include "Thread.h"
#include "LoginInfo.h"
#include "Monitor.h"

class CMonitorThread :
	public CThread
{
public:
	CMonitorThread();
	~CMonitorThread();
private:
	Monitor m_Monitor;
	SOCKET m_hSocket;
	sockaddr_in m_taddr = {};
	int m_iAddrSize;

public:
	bool Init();
	void Run();
};

