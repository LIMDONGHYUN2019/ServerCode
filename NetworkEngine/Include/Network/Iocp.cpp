#include "Iocp.h"

CIocp::CIocp()
{
	m_hComPort = 0;
	m_hStart = 0;
	m_iThreadCount = 0;
}

CIocp::~CIocp()
{
}

unsigned int __stdcall CIocp::ThreadCallback(void* pArg)
{
	CIocp* pIocp = (CIocp*)pArg;

	pIocp->IocpThread();

	return 0;
}

bool CIocp::Begin()
{
	m_hComPort = 0;

	SYSTEM_INFO	tSysInfo = {};
	GetSystemInfo(&tSysInfo);

	m_iThreadCount = (unsigned int)tSysInfo.dwNumberOfProcessors * 2;
	m_hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	if (!m_hComPort)
		return false;

	m_hStart = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (!m_hStart) // 
	{
		End();
		return false;
	}

	for (unsigned int i = 0; i < m_iThreadCount; ++i)
	{
		HANDLE	hThread = (HANDLE)_beginthreadex(nullptr, 0, CIocp::ThreadCallback, this, 0, 0);
		m_vecThread.push_back(hThread);

		WaitForSingleObject(m_hStart, INFINITE);
	}

	return true;
}

bool CIocp::End()
{
	for (unsigned int i = 0; i < m_iThreadCount; ++i)
	{
		// Iocp 큐에 메세지를 넣어주는 함수이다. 0을 넣어주어서 이 메세지를 받을 경우 Iocp 스레드를 종료하게 한다.
		PostQueuedCompletionStatus(m_hComPort, 0, 0, nullptr);
	}

	// 모든 스레드가 종료될때까지 대기한다.
	for (unsigned int i = 0; i < m_iThreadCount; ++i)
	{
		WaitForSingleObject(m_vecThread[i], INFINITE);
		CloseHandle(m_vecThread[i]);
	}

	if (m_hComPort)
		CloseHandle(m_hComPort);

	m_vecThread.clear();

	if (m_hStart)
		CloseHandle(m_hStart);

	return true;
}

bool CIocp::RegisterSocketToIocp(SOCKET hSocket, ULONG_PTR iCompletionKey)
{
	if (!hSocket || !iCompletionKey)
		return false;

	if (!CreateIoCompletionPort((HANDLE)hSocket, m_hComPort, iCompletionKey, 0))
		return false;

	return true;
}

void CIocp::IocpThread()
{
	OVERLAPPED* pOverlapped = nullptr;
	OverlappedEx* pOverlappedEx = nullptr;
	void* pObj = nullptr;
	void* pCompletionKey = nullptr;
	DWORD dwNumberOfTransfer = 0;
	BOOL	bSuccessed = FALSE;

	while (true)
	{
		SetEvent(m_hStart);

		bSuccessed = GetQueuedCompletionStatus(m_hComPort, &dwNumberOfTransfer, (PULONG_PTR)&pCompletionKey,
			&pOverlapped, INFINITE);
		
		if (!pCompletionKey)
			return;

		pOverlappedEx = (OverlappedEx*)pOverlapped;
		pObj = pOverlappedEx->pObject;

		if (!bSuccessed || (bSuccessed && !dwNumberOfTransfer))
		{
			if (pOverlappedEx->eIOType == IO_ACCEPT)
				OnIoConnected(pObj);

			else
				OnIoDisconnected(pObj);

			continue;
		}

		switch (pOverlappedEx->eIOType)
		{
		case IO_READ:
			OnIoRead(pObj, (unsigned int)dwNumberOfTransfer);
			break;
		case IO_WRITE:
			OnIoWrite(pObj, (unsigned int)dwNumberOfTransfer);
			break;
		}
	}
}
