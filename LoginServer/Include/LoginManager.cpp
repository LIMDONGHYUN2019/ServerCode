#include "LoginManager.h"
#include "NetworkEngine.h"
#include "LoginIocp.h"
#include "UserManager.h"
#include "DatabaseManager.h"
#include "MonitorThread.h"
#include "ThreadManager.h"

DEFINITION_SINGLE(CLoginManager)

CLoginManager::CLoginManager()	:
	m_pIocp(nullptr)
{
}

CLoginManager::~CLoginManager()
{
	if (m_pIocp)
		m_pIocp->End();

	DESTROY_SINGLE(CUserManager);

	DESTROY_SINGLE(CDatabaseManager);

	if (m_pListen)
	{
		m_pListen->End();
		SAFE_DELETE(m_pListen);
	}

	SAFE_DELETE(m_pIocp);

	DESTROY_SINGLE(CNetworkEngine);
}

bool CLoginManager::Init()
{
	if (!GET_SINGLE(CNetworkEngine)->Init())
	{
		return false;
	}

	if (!GET_SINGLE(CUserManager)->Init())
		return false;

	// Listen�� ������ �����.
	m_pListen = new CNetSession;

	// ���� ������ ���ε� �Ѵ�.
	if (!m_pListen->TcpBind())
		return false;

	if (!m_pListen->Listen(LOGIN_PORT, MAX_USER))
		return false;

	// Database ������ �ʱ�ȭ
	if (!GET_SINGLE(CDatabaseManager)->Init("localhost", "root", "dlaehdgus3!", "Game", 3306))
		return false;
	//---------------------------------------------------------------------------------------
	//if (!GET_SINGLE(CThreadManager)->InsertThread<CMonitorThread>("MonitorThread"))
	//	return false;

	//GET_SINGLE(CThreadManager)->FindThread("MonitorThread")->Start();
	//---------------------------------------------------------------------------------------

	// Login�� Iocp �� �����Ѵ�.
	m_pIocp = new CLoginIocp;

	m_pIocp->SetListenSession(m_pListen);

	m_pIocp->Begin();
	
	return true;
}

void CLoginManager::Run()
{
	//system("pause");
	getchar();
}
