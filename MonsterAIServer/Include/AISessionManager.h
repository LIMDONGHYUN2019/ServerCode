#pragma once

#include "AIInfo.h"

class CAISessionManager
{
private:
	std::vector<class CAISession*>	m_vecSession;

public:
	bool Init();
	bool Begin(SOCKET hListenSocket);

	DECLARE_SINGLE(CAISessionManager)
};

