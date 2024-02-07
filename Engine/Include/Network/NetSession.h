#pragma once

#include "../ThreadSync.h"
#include "../PacketQueue.h"

class CNetSession :
	public CThreadSync<CNetSession>
{
public:
	CNetSession();
	virtual ~CNetSession();

private:
	OverlappedEx	m_tAcceptOverlapped;
	OverlappedEx	m_tReadOverlapped;
	OverlappedEx	m_tWriteOverlapped;
	/*typedef struct _tagOverlappedEx
	{
		OVERLAPPED	tOverlapped;
		IO_TYPE		eIOType; // read, write, accept
		void* pObject;
	}OverlappedEx, * POverlappedEx;
	*/
	BYTE			m_ReadBuffer[MAX_BUFFER_LENGTH];
	SOCKADDR_IN		m_UDPRemoteAddr;

	SOCKET			m_hSocket;

	HANDLE			m_hUDPThreadHandle;
	HANDLE			m_hUDPThreadStartupEvent;
	HANDLE			m_hUDPThreadDestroyEvent;
	HANDLE			m_hUDPThreadWakeUpEvent;
	HANDLE			m_hUDPThreadCompleteEvent;

	bool			m_bUdpSending;

	CPacketQueue	m_SessionQueue;
	int		m_iUserNumber;

public:
	SOCKET GetSocket()	const
	{
		return m_hSocket; 
	}

	int GetUserNumber()	const
	{
		return m_iUserNumber;
		
	}

	void SetUserNumber(int iNumber)
	{
		m_iUserNumber = iNumber;
	}

public:
	virtual bool Begin();
	virtual bool End();
	bool Listen(USHORT sPort, int iBackLog);
	bool Accept(SOCKET hListenSocket);
	bool Connect(const char* pAddr, USHORT sPort);
	bool TcpBind();
	bool UdpBind();
	bool GetLocalIP(TCHAR* pIP)		const;
	unsigned short GetLocalPort()	const;
	bool InitializeReadForIocp();
	bool ReadForIocp(BYTE* pData, unsigned int& iLength);
	bool ReadForEventSelect(BYTE* pData, DWORD& dwLength);
	bool Write(BYTE* pData, DWORD dwLength);
};

