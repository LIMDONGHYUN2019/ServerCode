#include "AISessionManager.h"
#include "AISession.h"

DEFINITION_SINGLE(CAISessionManager)

CAISessionManager::CAISessionManager()
{
}

CAISessionManager::~CAISessionManager()
{
	for (size_t i = 0; i < m_vecSession.size(); ++i)
	{
		m_vecSession[i]->End();
		SAFE_DELETE(m_vecSession[i]);
	}

	m_vecSession.clear();
}

bool CAISessionManager::Init()
{
	return true;
}

bool CAISessionManager::Begin(SOCKET hListenSocket)
{
	m_vecSession.resize(100);
	// 유저를 생성해준다.
	for (int i = 0; i < 100; ++i)
	{
		CAISession* pUser = new CAISession;

		m_vecSession[i] = pUser;
	}

	for (int i = 0; i < 100; ++i)
	{
		if (!m_vecSession[i]->Begin())
		{
			return false;
		}

		// 비동기 Accept 상태로 만들어주어서 접속되는순간 이 세션에 소켓을 할당해준다.
		if (!m_vecSession[i]->Accept(hListenSocket))
		{
			return false;
		}
	}

	return true;
}
