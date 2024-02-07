#include "Thread.h"

CThread::CThread()
{
	m_hThread = 0;
	m_Loop = true;
}

CThread::~CThread()
{
	if (m_hThread)
	{
		m_Loop = false;
		SetEvent(m_hStart);
		CloseHandle(m_hStart);

		WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hThread);
	}
}

bool CThread::Create()
{
	m_hStart = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, CThread::ThreadFunc, this, 0, 0);

	return true;
}

void CThread::StartPending()
{
	WaitForSingleObject(m_hStart, INFINITE);
}

void CThread::Start()
{
	SetEvent(m_hStart);
}

UINT __stdcall CThread::ThreadFunc(void* pArg)
{
	CThread* pThread = (CThread*)pArg;

	pThread->StartPending();

	pThread->Run();

	return 0;
}
