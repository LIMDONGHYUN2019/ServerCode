#pragma once

#include "ThreadSync.h"
/*
typedef struct _tagSessionQueueData
{
	void* pObject;
	BYTE	Data[MAX_BUFFER_LENGTH];
	DWORD	dwLength;
	char	cRemoteAddr[32];
	USHORT	sRemotePort;
	DWORD	dwProtocol;
}SessionQueueData, *PSessionQueueData;
*/

#define	MAX_QUEUE_LENGTH	500

class CPacketQueue	:
	public CThreadSync<CPacketQueue>
{
public:
	CPacketQueue();
	~CPacketQueue();

private:
	SessionQueueData	m_Queue[MAX_QUEUE_LENGTH];
	// IOCP ÆÐÅ¶
	int			m_iHead;
	int			m_iTail;

public:
	bool Begin();
	bool End();

	BYTE* Push(void* pObj, BYTE* pData, unsigned int iLength);
	BYTE* Push(void* pObj, BYTE* pData, unsigned int iLength, char* pAddress, unsigned short sPort);
	BYTE* Push(void* pObj, unsigned int iProtocol, BYTE* pData, unsigned int iLength);
	BYTE* Push(void* pObj, unsigned int iProtocol, BYTE* pData, unsigned int iLength, char* pAddress, unsigned short sPort);

	bool Pop();
	bool Pop(void** pObj, BYTE* pData, unsigned int& dwLength);
	bool Pop(void** pObj, BYTE* pData, unsigned int& dwLength, char* pAddress, unsigned short& sPort);
	bool Pop(void** pObj, unsigned int& iProtocol, BYTE* pData, unsigned int& iLength);
	bool Pop(void** pObj, unsigned int& iProtocol, BYTE* pData, unsigned int& iLength, char* pAddress, unsigned short& sPort);

	bool Empty();
};
