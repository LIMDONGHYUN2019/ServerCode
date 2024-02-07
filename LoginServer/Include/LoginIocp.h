#pragma once

#include "LoginInfo.h"
#include "Network/Iocp.h"

class CLoginIocp :
	public CIocp
{
public:
	CLoginIocp();
	virtual ~CLoginIocp();

private:
	class CNetSession* m_pListenSession;
	class CUserManager* m_pUserManager;

public:
	void SetListenSession(class CNetSession* pSession)
	{
		m_pListenSession = pSession;
	}

public:
	virtual bool Begin();
	virtual bool End();

protected:
	virtual void OnIoRead(void* pObj, unsigned int iLength);
	virtual void OnIoWrite(void* pObj, unsigned int iLength);
	virtual void OnIoConnected(void* pObj);
	virtual void OnIoDisconnected(void* pObj);

private:
	void Membership(BYTE* pPacket, class CUserSession* pSession);
	void Login(BYTE* pPacket, class CUserSession* pSession);
};

