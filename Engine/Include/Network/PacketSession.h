#pragma once

#include "NetSession.h"

typedef struct _tagReadPacketInfo
{
	char	strAddress[14];
	unsigned short	sPort;
	unsigned int	iPacketNumber;
}ReadPacketInfo, *PReadPacketInfo;

class CPacketSession :
	public CNetSession
{
public:
	CPacketSession();
	virtual ~CPacketSession();

private:
	BYTE			m_PacketBuffer[MAX_BUFFER_LENGTH * 3]; // 일부러 넉넉히
	int				m_iRemainLength; // Tcp제어정보의 시퀀스번호라고 하자.
	unsigned int			m_iCurrentPacketNumber; // 유사 Ack번호라고 하자.
	unsigned int			m_iLastReadPacketNumber;

	CPacketQueue	m_WriteQueue;

	vector<ReadPacketInfo>			m_vecReadPacketInfoUdp;
public:
	virtual bool Begin();
	virtual bool End();
	bool ReadPacketForIocp(unsigned int& iLength);
	bool ReadPacketForEventSelect();
	bool WritePacket(int iProtocol, const BYTE* pPacket, unsigned int iPacketLength);
	bool WriteComplete();
	bool GetPacket(unsigned int& iProtocol, BYTE* pPacket, unsigned int& iPacketLength);
};
