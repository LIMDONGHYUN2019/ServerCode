#pragma once

#include "AIInfo.h"
#include "Network/Iocp.h"

class CAIIocp :
    public CIocp
{
public:
	CAIIocp();
	virtual ~CAIIocp();

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
	void FindPath(BYTE* pPacket, class CPacketSession* pSession);
};

