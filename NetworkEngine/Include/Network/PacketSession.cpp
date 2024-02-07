#include "PacketSession.h"
#include "../Crypt.h"

CPacketSession::CPacketSession()
{
	memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));

	m_iRemainLength = 0;
	m_iCurrentPacketNumber = 0;
	m_iLastReadPacketNumber = 0;
}

CPacketSession::~CPacketSession()
{
}

bool CPacketSession::Begin()
{
	CThreadSync	sync;

	memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));

	m_iRemainLength = 0;
	m_iCurrentPacketNumber = 0;
	m_iLastReadPacketNumber = 0;

	return CNetSession::Begin();
}

bool CPacketSession::End()
{
	CThreadSync	sync;

	m_vecReadPacketInfoUdp.clear();

	return CNetSession::End();
}

bool CPacketSession::ReadPacketForIocp(unsigned int& iLength)
{
	CThreadSync	sync;

	if (!CNetSession::ReadForIocp(m_PacketBuffer + m_iRemainLength, iLength)) //
		return false;

	m_iRemainLength += iLength;

	return true;
}

bool CPacketSession::ReadPacketForEventSelect()
{
	CThreadSync	sync;
	DWORD	dwReadLength = 0;

	if (!CNetSession::ReadForEventSelect(m_PacketBuffer + m_iRemainLength, dwReadLength))
		return false;

	m_iRemainLength += dwReadLength;

	return true;
}

bool CPacketSession::WritePacket(int iProtocol, const BYTE* pPacket, unsigned int iPacketLength)
{
	CThreadSync	sync;
	if (!pPacket)
		return false;

	// ��Ŷ�� ����(4����Ʈ) | ��Ŷ��ȣ(4����Ʈ) | ��������(4����Ʈ) | ����(4096)
	unsigned int iLength = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int)
		+ iPacketLength;
	// ���̸� ������ ��Ŷ�� ��ȣȭ�Ѵ�.

	if (iLength > MAX_BUFFER_LENGTH + 12)
		return false;

	// ��Ŷ��ȣ�� �����Ѵ�.
	++m_iCurrentPacketNumber;

	BYTE	Temp[MAX_BUFFER_LENGTH] = {};

	// ��Ŷ ���̸� �־��ش�.
	memcpy(Temp, &iLength, sizeof(unsigned int));
	// ���̕��� ������ ��ȣȭ
	// �޸� ������ ���� ����. �׷��Ƿ� ��Ŷ ��ȣ�� ������ �����̹Ƿ� ���� ������ �ٿ��ش�.
	memcpy(Temp + sizeof(unsigned int), &m_iCurrentPacketNumber, sizeof(unsigned int));

	// ���������� �־��ش�.
	memcpy(Temp + (sizeof(unsigned int) * 2), &iProtocol, sizeof(int));

	// Packet�� �ٿ��ش�.
	memcpy(Temp + (sizeof(unsigned int) * 3), pPacket, iPacketLength);

	// ��ȣȭ�Ѵ�.
	CCrypt::EnCrypt((char*)Temp + sizeof(unsigned int), (char*)Temp + sizeof(unsigned int),
		iLength - sizeof(unsigned int));

	m_WriteQueue.Push(this, Temp, iLength);
	return CNetSession::Write(Temp, iLength);
}

bool CPacketSession::WriteComplete()
{
	CThreadSync	sync;

	return m_WriteQueue.Pop();
}

bool CPacketSession::GetPacket(unsigned int& iProtocol, BYTE* pPacket, unsigned int& iPacketLength)
{
	CThreadSync	sync;

	if (!pPacket)
		return false;

	if (m_iRemainLength < sizeof(unsigned int))
		return false;

	// ��Ŷ�� ����(4����Ʈ) | ��Ŷ��ȣ(4����Ʈ) | ��������(4����Ʈ) | ����(4096)
	int iLength = 0;
	memcpy(&iLength, m_PacketBuffer, sizeof(unsigned int));
	// ��ü ��Ŷ���� ��������
	if (iLength > MAX_BUFFER_LENGTH || iLength < 0)
	{
		m_iRemainLength = 0;
		return false;
	} 
	// �� ó�� Ư�� Session�� ��üũ���̻�ŭ ���ۿ��� ���������� �׋����� ���ڴ�.
	if (iLength <= m_iRemainLength)
	{
		// ��ȣȭ�� ��Ŷ�� ��ȣȭ ���ش�.
		CCrypt::DeCrypt((char*)m_PacketBuffer + sizeof(unsigned int), (char*)m_PacketBuffer + sizeof(unsigned int),
			iLength - sizeof(unsigned int));

		// ��Ŷ�� �������ش�.
		unsigned int	iPacketNumber = 0;
		unsigned int	iProtocolNumber = 0;

		memcpy(&iPacketNumber, m_PacketBuffer + sizeof(unsigned int), sizeof(unsigned int));
		memcpy(&iProtocolNumber, m_PacketBuffer + sizeof(unsigned int) + sizeof(unsigned int), sizeof(unsigned int));

		iProtocol = iProtocolNumber;
		iPacketLength = iLength - sizeof(unsigned int) - sizeof(unsigned int) - sizeof(unsigned int);

		memcpy(pPacket, m_PacketBuffer + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int),
			iPacketLength);
		// ���⼭ �˼��ִ°��� �������� ��Ŷ���� Session���� �����ٵ� �ش� ��Ŷ�� �������� 
		// PacketSession�� ����ҽ�����ġ�� �Ű��ְ� RemainLength�� PacketSession�� ���� ���� Session ũ��ϱ�
		// �ܺηκ��� ���� ũ�⸸ŭ ���ش�. �翬�� Ư�� Sessionũ�⸦ ���� �о����� 
		// ���� ���� �� ������ �ϴ� �ʱ�ȭ���ش�. ����ҳ� RemainLength���
		// �׷��� ������� PacketBufferũ�⸦ *3�� ���ذ��̴�.
		if (m_iRemainLength - iLength > 0)
			memmove(m_PacketBuffer, m_PacketBuffer + iLength, m_iRemainLength - iLength);
		//memmove a,b ũ�� -> a ��ġ���� b�޸𸮸� ����ũ�⸸ŭ �ٿ��ش�.
		m_iRemainLength -= iLength;

		if (m_iRemainLength <= 0)
		{
			m_iRemainLength = 0;
			memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
		}

		return true;

		// ������ ��Ŷ ��ȣ���� �۰ų� ���� ����� �߸��� ���̹Ƿ� �����Ѵ�.
		if (iPacketNumber <= m_iLastReadPacketNumber)
			return false;

		else
		{
			m_iLastReadPacketNumber = iPacketNumber;

			return true;
		}
	}

	return false;
}
