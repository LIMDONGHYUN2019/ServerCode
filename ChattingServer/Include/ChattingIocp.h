#pragma once

#include "ChattingInfo.h"
#include "UserManager.h"
#include "Network/Iocp.h"

class CChattingIocp :
	public CIocp
{
public:
	CChattingIocp();
	virtual ~CChattingIocp();

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
	void Message(BYTE* pPacket, class CUser* pSession);
	void Login(BYTE* pPacket, class CUser* pSession);

protected:
	virtual void OnIoRead(void* pObj, unsigned int iLength);
	virtual void OnIoWrite(void* pObj, unsigned int iLength);
	virtual void OnIoConnected(void* pObj);
	virtual void OnIoDisconnected(void* pObj);
};

