#include "PacketQueue.h"

CPacketQueue::CPacketQueue()
{
	memset(m_Queue, 0, sizeof(m_Queue));
	m_iHead = 0;
	m_iTail = 0;
}

CPacketQueue::~CPacketQueue()
{
}

bool CPacketQueue::Begin()
{
	memset(m_Queue, 0, sizeof(m_Queue));
	m_iHead = 0;
	m_iTail = 0;

	return true;
}

bool CPacketQueue::End()
{
	return true;
}

BYTE* CPacketQueue::Push(void* pObj, BYTE* pData, unsigned int iLength)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return nullptr;

	unsigned int iTail = (m_iTail + 1) % MAX_QUEUE_LENGTH;

	if (iTail == m_iHead)
		return nullptr;

	m_Queue[iTail].pObject = pObj;
	m_Queue[iTail].dwLength = (DWORD)iLength;

	memset(m_Queue[iTail].Data, 0, sizeof(m_Queue[iTail].Data));
	memcpy(m_Queue[iTail].Data, pData, iLength);

	m_iTail = iTail;

	return m_Queue[iTail].Data; // 패킷 주소
}

BYTE* CPacketQueue::Push(void* pObj, BYTE* pData, unsigned int iLength, char* pAddress, unsigned short sPort)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return nullptr;

	unsigned int iTail = (m_iTail + 1) % MAX_QUEUE_LENGTH;

	if (iTail == m_iHead)
		return nullptr;

	m_Queue[iTail].pObject = pObj;
	m_Queue[iTail].dwLength = (DWORD)iLength;
	m_Queue[iTail].sRemotePort = sPort;
	memset(m_Queue[iTail].cRemoteAddr, 0, sizeof(m_Queue[iTail].cRemoteAddr));
	strcpy_s(m_Queue[iTail].cRemoteAddr, pAddress);

	memset(m_Queue[iTail].Data, 0, sizeof(m_Queue[iTail].Data));
	memcpy(m_Queue[iTail].Data, pData, iLength);

	m_iTail = iTail;

	return m_Queue[iTail].Data;
}

BYTE* CPacketQueue::Push(void* pObj, unsigned int iProtocol, BYTE* pData, unsigned int iLength)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return nullptr;

	unsigned int iTail = (m_iTail + 1) % MAX_QUEUE_LENGTH;

	if (iTail == m_iHead)
		return nullptr;

	m_Queue[iTail].pObject = pObj;
	m_Queue[iTail].dwLength = (DWORD)iLength;
	m_Queue[iTail].dwProtocol = (DWORD)iProtocol;

	memset(m_Queue[iTail].Data, 0, sizeof(m_Queue[iTail].Data));
	memcpy(m_Queue[iTail].Data, pData, iLength);

	m_iTail = iTail;

	return m_Queue[iTail].Data;
}

BYTE* CPacketQueue::Push(void* pObj, unsigned int iProtocol, BYTE* pData, unsigned int iLength, char* pAddress, unsigned short sPort)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return nullptr;

	unsigned int iTail = (m_iTail + 1) % MAX_QUEUE_LENGTH;

	if (iTail == m_iHead)
		return nullptr;

	m_Queue[iTail].pObject = pObj;
	m_Queue[iTail].dwLength = (DWORD)iLength;
	m_Queue[iTail].sRemotePort = sPort;
	m_Queue[iTail].dwLength = (DWORD)iProtocol;

	memset(m_Queue[iTail].cRemoteAddr, 0, sizeof(m_Queue[iTail].cRemoteAddr));
	strcpy_s(m_Queue[iTail].cRemoteAddr, pAddress);

	memset(m_Queue[iTail].Data, 0, sizeof(m_Queue[iTail].Data));
	memcpy(m_Queue[iTail].Data, pData, iLength);

	m_iTail = iTail;

	return m_Queue[iTail].Data;
}

bool CPacketQueue::Pop()
{
	CThreadSync	sync;

	if (m_iHead == m_iTail)
		return false;

	m_iHead = (m_iHead + 1) % MAX_QUEUE_LENGTH;

	return true;
}

bool CPacketQueue::Pop(void** pObj, BYTE* pData, unsigned int& dwLength)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return false;

	if (m_iHead == m_iTail)
		return false;

	unsigned int iHead = (m_iHead + 1) % MAX_QUEUE_LENGTH;

	dwLength = (unsigned int)m_Queue[iHead].dwLength;
	*pObj = m_Queue[iHead].pObject;

	memcpy(pData, m_Queue[iHead].Data, m_Queue[iHead].dwLength);

	m_iHead = iHead;

	return true;
}

bool CPacketQueue::Pop(void** pObj, BYTE* pData, unsigned int& dwLength, char* pAddress, unsigned short& sPort)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return false;

	if (m_iHead == m_iTail)
		return false;

	unsigned int iHead = (m_iHead + 1) % MAX_QUEUE_LENGTH;

	dwLength = (unsigned int)m_Queue[iHead].dwLength;
	*pObj = m_Queue[iHead].pObject;
	sPort = m_Queue[iHead].sRemotePort;

	strcpy_s(pAddress, sizeof(m_Queue[iHead].cRemoteAddr), m_Queue[iHead].cRemoteAddr);

	memcpy(pData, m_Queue[iHead].Data, m_Queue[iHead].dwLength);

	m_iHead = iHead;

	return true;
}

bool CPacketQueue::Pop(void** pObj, unsigned int& iProtocol, BYTE* pData, unsigned int& iLength)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return false;

	if (m_iHead == m_iTail)
		return false;

	unsigned int iHead = (m_iHead + 1) % MAX_QUEUE_LENGTH;

	iProtocol = (unsigned int)m_Queue[iHead].dwProtocol;
	iLength = (unsigned int)m_Queue[iHead].dwLength;
	*pObj = m_Queue[iHead].pObject;

	memcpy(pData, m_Queue[iHead].Data, m_Queue[iHead].dwLength);

	m_iHead = iHead;

	return true;
}

bool CPacketQueue::Pop(void** pObj, unsigned int& iProtocol, BYTE* pData, unsigned int& iLength, char* pAddress, unsigned short& sPort)
{
	CThreadSync	sync;

	if (!pObj || !pData)
		return false;

	if (m_iHead == m_iTail)
		return false;

	unsigned int iHead = (m_iHead + 1) % MAX_QUEUE_LENGTH;

	iProtocol = (unsigned int)m_Queue[iHead].dwProtocol;
	iLength = (unsigned int)m_Queue[iHead].dwLength;
	*pObj = m_Queue[iHead].pObject;
	sPort = m_Queue[iHead].sRemotePort;

	strcpy_s(pAddress, sizeof(m_Queue[iHead].cRemoteAddr), m_Queue[iHead].cRemoteAddr);

	memcpy(pData, m_Queue[iHead].Data, m_Queue[iHead].dwLength);

	m_iHead = iHead;

	return true;
}

bool CPacketQueue::Empty()
{
	CThreadSync	sync;

	if (m_iHead == m_iTail)
		return true;

	return false;
}
