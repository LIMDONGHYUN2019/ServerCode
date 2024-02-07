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
	BYTE			m_PacketBuffer[MAX_BUFFER_LENGTH * 3]; // �Ϻη� �˳���
	int				m_iRemainLength; // Tcp���������� ��������ȣ��� ����.
	unsigned int			m_iCurrentPacketNumber; // ���� Ack��ȣ��� ����.
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
