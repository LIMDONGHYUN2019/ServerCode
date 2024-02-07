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

	// 패킷은 길이(4바이트) | 패킷번호(4바이트) | 프로토콜(4바이트) | 내용(4096)
	unsigned int iLength = sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int)
		+ iPacketLength;
	// 길이를 제외한 패킷을 암호화한다.

	if (iLength > MAX_BUFFER_LENGTH + 12)
		return false;

	// 패킷번호를 증가한다.
	++m_iCurrentPacketNumber;

	BYTE	Temp[MAX_BUFFER_LENGTH] = {};

	// 패킷 길이를 넣어준다.
	memcpy(Temp, &iLength, sizeof(unsigned int));
	// 길이뺴고 나머지 암호화
	// 메모리 구조는 위와 같다. 그러므로 패킷 번호는 길이의 다음이므로 길이 다음에 붙여준다.
	memcpy(Temp + sizeof(unsigned int), &m_iCurrentPacketNumber, sizeof(unsigned int));

	// 프로토콜을 넣어준다.
	memcpy(Temp + (sizeof(unsigned int) * 2), &iProtocol, sizeof(int));

	// Packet을 붙여준다.
	memcpy(Temp + (sizeof(unsigned int) * 3), pPacket, iPacketLength);

	// 암호화한다.
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

	// 패킷은 길이(4바이트) | 패킷번호(4바이트) | 프로토콜(4바이트) | 내용(4096)
	int iLength = 0;
	memcpy(&iLength, m_PacketBuffer, sizeof(unsigned int));
	// 전체 패킷길이 가져오고
	if (iLength > MAX_BUFFER_LENGTH || iLength < 0)
	{
		m_iRemainLength = 0;
		return false;
	} 
	// 맨 처음 특정 Session의 전체크기이상만큼 버퍼에서 가져왔으면 그떄서야 얻어가겠다.
	if (iLength <= m_iRemainLength)
	{
		// 암호화된 패킷을 복호화 해준다.
		CCrypt::DeCrypt((char*)m_PacketBuffer + sizeof(unsigned int), (char*)m_PacketBuffer + sizeof(unsigned int),
			iLength - sizeof(unsigned int));

		// 패킷을 복사해준다.
		unsigned int	iPacketNumber = 0;
		unsigned int	iProtocolNumber = 0;

		memcpy(&iPacketNumber, m_PacketBuffer + sizeof(unsigned int), sizeof(unsigned int));
		memcpy(&iProtocolNumber, m_PacketBuffer + sizeof(unsigned int) + sizeof(unsigned int), sizeof(unsigned int));

		iProtocol = iProtocolNumber;
		iPacketLength = iLength - sizeof(unsigned int) - sizeof(unsigned int) - sizeof(unsigned int);

		memcpy(pPacket, m_PacketBuffer + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int),
			iPacketLength);
		// 여기서 알수있는것은 여러개의 패킷들이 Session에서 들어올텐데 해당 패킷을 다읽으면 
		// PacketSession의 저장소시작위치를 옮겨주고 RemainLength는 PacketSession에 들어온 여러 Session 크기니까
		// 외부로부터 읽힌 크기만큼 뺴준다. 당연히 특정 Session크기를 전부 읽었을떄 
		// 물론 전부 다 읽으면 싹다 초기화해준다. 저장소나 RemainLength등등
		// 그래서 저장소인 PacketBuffer크기를 *3을 해준것이다.
		if (m_iRemainLength - iLength > 0)
			memmove(m_PacketBuffer, m_PacketBuffer + iLength, m_iRemainLength - iLength);
		//memmove a,b 크기 -> a 위치에다 b메모리를 인자크기만큼 붙여준다.
		m_iRemainLength -= iLength;

		if (m_iRemainLength <= 0)
		{
			m_iRemainLength = 0;
			memset(m_PacketBuffer, 0, sizeof(m_PacketBuffer));
		}

		return true;

		// 이전의 패킷 번호보다 작거나 같을 경우라면 잘못된 것이므로 무시한다.
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
