#pragma once

#include "../Engine.h"

class CEventSelect
{
public:
	CEventSelect();
	virtual ~CEventSelect();

private:
	HANDLE		m_StartupEvent;
	HANDLE		m_DestroyEvent;
	HANDLE		m_hSelectEvent;
	HANDLE		m_hSelectThread;

	SOCKET		m_hSocket;

protected:
	virtual void OnIoRead() = 0;
	virtual void OnIoConnected() = 0;
	virtual void OnIoDisconnected() = 0;

public:
	bool Begin(SOCKET hSocket);
	bool End();
	void ThreadCallback();

private:
	static unsigned int __stdcall ThreadCall(void* pArg);
};
