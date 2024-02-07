#pragma once

#include "ChattingInfo.h"
#include "Network/NetSession.h"

class CNetworkManager
{
private:
	class CChattingIocp* m_pIocp;
	class CNetSession* m_pListen;

public:
	bool Init();
	void Run();

	DECLARE_SINGLE(CNetworkManager)
};

