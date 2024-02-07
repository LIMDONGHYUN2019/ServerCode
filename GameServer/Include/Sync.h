#pragma once

#include "CriticalSection.h"

class CSectionSync
{
public:
	CSectionSync(CCriticalSection* Crt)
	{
		m_Crt = Crt;
		m_Crt->Lock();
	}

	~CSectionSync()
	{
		m_Crt->UnLock();
	}

private:
	CCriticalSection* m_Crt;
};

