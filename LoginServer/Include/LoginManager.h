#pragma once

#include "LoginInfo.h"
#include "Network/NetSession.h"

class CLoginManager
{
private:
	class CLoginIocp* m_pIocp;
	class CNetSession* m_pListen;

public:
	bool Init();
	void Run();

	DECLARE_SINGLE(CLoginManager)
};                  