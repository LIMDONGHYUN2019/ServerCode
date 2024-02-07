#include "ThreadManager.h"
#include "Thread.h"

DEFINITION_SINGLE(CThreadManager)

CThreadManager::CThreadManager()
{
}

CThreadManager::~CThreadManager()
{
	SAFE_DELETE_MAP(m_mapThread);
}