#pragma once

#include "CriticalSection.h"

template <typename T>
class CThreadSync
{
public:
	CThreadSync()
	{
	}

	virtual ~CThreadSync()
	{
	}

public:
	class CSync
	{
	public:
		CSync()
		{
			T::m_Sync.Lock();
		}

		~CSync()
		{
			T::m_Sync.UnLock();
		}
	};

private:
	static CCriticalSection	m_Sync;
};

template <typename T>
CCriticalSection CThreadSync<T>::m_Sync;

