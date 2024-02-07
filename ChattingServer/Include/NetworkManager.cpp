#include "NetworkManager.h"
#include "NetworkEngine.h"
#include "ChattingIocp.h"
#include "UserManager.h"

DEFINITION_SINGLE(CNetworkManager)

CNetworkManager::CNetworkManager() :
	m_pIocp(nullptr),
	m_pListen(nullptr)
{
}

CNetworkManager::~CNetworkManager()
{
	DESTROY_SINGLE(CUserManager);

	if (m_pIocp)
		m_pIocp->End();

	if (m_pListen)
	{
		m_pListen->End();
		SAFE_DELETE(m_pListen);
	}

	SAFE_DELETE(m_pIocp);

	DESTROY_SINGLE(CNetworkEngine);
}

bool CNetworkManager::Init()
{
	if (!GET_SINGLE(CNetworkEngine)->Init())
	{
		return false;
	}

	if (!GET_SINGLE(CUserManager)->Init())
		return false;

	// Listen용 세션을 만든다.
	m_pListen = new CNetSession;

	// 서버 소켓을 바인딩 한다.
	if (!m_pListen->TcpBind())
		return false;

	if (!m_pListen->Listen(CHATTING_PORT, MAX_USER ))
		return false;

	// Chatting용 Iocp 를 생성한다.
	m_pIocp = new CChattingIocp;

	m_pIocp->SetListenSession(m_pListen);

	m_pIocp->Begin();

	return true;
}

void CNetworkManager::Run()
{
	//system("pause");
	getchar();
}

