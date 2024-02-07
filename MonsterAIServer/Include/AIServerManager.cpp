#include "AIServerManager.h"
#include "Navigation/NavigationManager.h"
#include "NetworkEngine.h"
#include "Level/LevelManager.h"
#include "Network/NetSession.h"
#include "Network/PacketSession.h"
#include "AIIocp.h"
#include "AISessionManager.h"

DEFINITION_SINGLE(CAIServerManager)

CAIServerManager::CAIServerManager()	:
	m_Iocp(nullptr),
	m_Loop(true)
{
}

CAIServerManager::~CAIServerManager()
{
	CNavigationManager::DestroyInst();

	if (m_Iocp)
		m_Iocp->End();

	if (m_pListen)
	{
		m_pListen->End();
		SAFE_DELETE(m_pListen);
	}

	SAFE_DELETE(m_Iocp);

	CAISessionManager::DestroyInst();
	DESTROY_SINGLE(CNetworkEngine);
}

bool CAIServerManager::Init()
{
	if (!GET_SINGLE(CNetworkEngine)->Init())
	{
		return false;
	}

	if (!CAISessionManager::GetInst()->Init())
		return false;

	if (!CNavigationManager::GetInst()->Init())
		return false;

	if (!CLevelManager::GetInst()->Init())
		return false;

	// Listen용 세션을 만든다.
	m_pListen = new CNetSession;

	// 서버 소켓을 바인딩 한다.
	if (!m_pListen->TcpBind())
		return false;

	if (!m_pListen->Listen(AISERVER_PORT, 100))
		return false;

	m_Iocp = new CAIIocp;

	m_Iocp->SetListenSession(m_pListen);

	m_Iocp->Begin();

	return true;
}

void CAIServerManager::Run()
{
	while (m_Loop)
	{
		// 유저 목록을 반복하며 이동중인 유저들은 이동 동기화 작업을 시작한다.
		//m_pTimer->Update();

		// Queue에 데이터가 있는지 판단한다.
		// 패킷을 빠르게 못받아올거같다. 패킷한번받아오고 매니저들 업데이트 한번하고
		if (!m_PacketQueue.empty())
		{
		}
	}


}
