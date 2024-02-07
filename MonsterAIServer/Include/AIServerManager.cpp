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

	// Listen�� ������ �����.
	m_pListen = new CNetSession;

	// ���� ������ ���ε� �Ѵ�.
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
		// ���� ����� �ݺ��ϸ� �̵����� �������� �̵� ����ȭ �۾��� �����Ѵ�.
		//m_pTimer->Update();

		// Queue�� �����Ͱ� �ִ��� �Ǵ��Ѵ�.
		// ��Ŷ�� ������ ���޾ƿðŰ���. ��Ŷ�ѹ��޾ƿ��� �Ŵ����� ������Ʈ �ѹ��ϰ�
		if (!m_PacketQueue.empty())
		{
		}
	}


}
