#pragma once

#include "Engine.h"

class CThreadManager
{
private:
	unordered_map<size_t, class CThread*>	m_mapThread;

public:
	template <typename T>
	bool InsertThread(const string& strName)//createThread
	{
		T* pThread = (T*)FindThread(strName);

		if (pThread)
			return false;

		pThread = new T; 

		if (!pThread->Create())
		{
			SAFE_DELETE(pThread);
			return false; 
		}

		hash<string>	h;
		size_t iKey = h(strName);

		m_mapThread.insert(make_pair(iKey, (CThread*)pThread));

		return true;
	}

public:
	class CThread* FindThread(const string& strName)
	{
		hash<string>	h;
		size_t iKey = h(strName);

		auto	iter = m_mapThread.find(iKey);

		if (iter == m_mapThread.end())
			return nullptr;

		return iter->second;
	}

	DECLARE_SINGLE(CThreadManager)
};

