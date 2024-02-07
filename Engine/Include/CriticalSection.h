#pragma once

#include "Engine.h"

class CCriticalSection
{
public:
	CCriticalSection()
	{
		InitializeCriticalSection(&m_tCrt);
	}

	~CCriticalSection()
	{
		DeleteCriticalSection(&m_tCrt);
	}

public:
	inline void Lock()
	{
		EnterCriticalSection(&m_tCrt);
	}

	inline void UnLock()
	{
		LeaveCriticalSection(&m_tCrt);
	}

private:
	CRITICAL_SECTION	m_tCrt;
};

