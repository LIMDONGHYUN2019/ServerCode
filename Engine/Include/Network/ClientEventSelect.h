#pragma once

#include "EventSelect.h"
#include "../PacketQueue.h"

class CClientEventSelect :
	public CEventSelect
{
public:
	CClientEventSelect();
	virtual ~CClientEventSelect();

protected:
	class CPacketSession* m_pSession;
	CPacketQueue		m_ReadQueue;

public:
	bool BeginTCP(const char* pIP, short sPort);
	bool EndTCP();
	bool ReadPacket(unsigned int& iProtocol, BYTE* pData, unsigned int& iLength);
	bool WritePacket(unsigned int iProtocol, const BYTE* pData, unsigned int iLength);

protected:
	virtual void OnIoRead();
	virtual void OnIoConnected();
	virtual void OnIoDisconnected();
};

