#pragma once

#include "Engine.h"

class CThread
{
	friend class CThreadManager;

protected:
	CThread();
	virtual ~CThread();

protected:
	HANDLE	m_hThread;
	HANDLE	m_hStart;
	char	m_strName[NAME_SIZE];
	bool	m_Loop;

public:
	bool Create();
	virtual void Run() = 0;
	void StartPending();
	void Start();

private:
	static UINT __stdcall ThreadFunc(void* pArg);
};

