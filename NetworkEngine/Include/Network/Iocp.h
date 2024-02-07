#pragma once

#include "../Engine.h"

class CIocp
{
public:
	CIocp();
	virtual ~CIocp();

private:
	HANDLE			m_hComPort;
	unsigned int	m_iThreadCount;

	vector<HANDLE>	m_vecThread;
	HANDLE			m_hStart;

protected:
	virtual void OnIoRead(void* pObj, unsigned int iLength) = 0;
	virtual void OnIoWrite(void* pObj, unsigned int iLength) = 0;
	virtual void OnIoConnected(void* pObj) = 0;
	virtual void OnIoDisconnected(void* pObj) = 0;

public:
	virtual bool Begin();
	virtual bool End();
	bool RegisterSocketToIocp(SOCKET hSocket, ULONG_PTR iCompletionKey);
	void IocpThread();

private:
	static unsigned int __stdcall ThreadCallback(void* pArg);
};

