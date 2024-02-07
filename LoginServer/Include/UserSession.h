#pragma once

#include "LoginInfo.h"
#include "Network/PacketSession.h"

class CUserSession :
	public CPacketSession
{
public:
	CUserSession();
	~CUserSession();
};

