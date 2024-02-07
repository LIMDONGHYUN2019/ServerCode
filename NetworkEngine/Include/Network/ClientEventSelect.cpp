
#include "ClientEventSelect.h"
#include "PacketSession.h"

CClientEventSelect::CClientEventSelect()	:
	m_pSession(nullptr)
{
}

CClientEventSelect::~CClientEventSelect()
{
	if (m_pSession)
	{
		m_pSession->End();
		SAFE_DELETE(m_pSession);
	}
}

bool CClientEventSelect::BeginTCP(const char* pIP, short sPort)
{
	if (!pIP || sPort <= 0 || m_pSession)
		return false;

	m_pSession = new CPacketSession;

	if (!m_pSession->Begin())
	{
		EndTCP();
		return false;
	}

	if (!m_pSession->TcpBind())
	{
		EndTCP();
		return false;
	}

	if (!Begin(m_pSession->GetSocket()))
	{
		EndTCP();
		return false;
	}

	if (!m_pSession->Connect(pIP, sPort))
	{
		EndTCP();
		return false;
	}

	return true;
}

bool CClientEventSelect::EndTCP()
{
	if (m_pSession)
		m_pSession->End();

	SAFE_DELETE(m_pSession);

	End();

	return true;
}

bool CClientEventSelect::ReadPacket(unsigned int& iProtocol, BYTE* pData, unsigned int& iLength)
{
	void* pObj = nullptr;

	return m_ReadQueue.Pop(&pObj, iProtocol, pData, iLength);
}

bool CClientEventSelect::WritePacket(unsigned int iProtocol, const BYTE* pData, unsigned int iLength)
{
	if (!m_pSession->WritePacket(iProtocol, pData, iLength))
		return false;

	if (!m_pSession->WriteComplete())
		return false;

	return true;
}

void CClientEventSelect::OnIoRead()
{
	BYTE	packet[MAX_BUFFER_LENGTH] = {};
	unsigned int iLength = 0;
	unsigned int iProtocol = 0;

	if (m_pSession->ReadPacketForEventSelect())
	{
		while (m_pSession->GetPacket(iProtocol, packet, iLength))
		{
			m_ReadQueue.Push(this, iProtocol, packet, iLength);
		}
	}
}

void CClientEventSelect::OnIoConnected()
{
}

void CClientEventSelect::OnIoDisconnected()
{
}
