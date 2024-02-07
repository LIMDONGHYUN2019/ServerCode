#include "NetworkEngine.h"
#include "ThreadManager.h"
#include "PathManager.h"

DEFINITION_SINGLE(CNetworkEngine)

CNetworkEngine::CNetworkEngine()
{
	WSADATA		wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

CNetworkEngine::~CNetworkEngine()
{
	DESTROY_SINGLE(CThreadManager);

	WSACleanup();
}

bool CNetworkEngine::Init()
{
	GET_SINGLE(CPathManager)->Init();

	return true;
}

void CNetworkEngine::Run()
{
}
