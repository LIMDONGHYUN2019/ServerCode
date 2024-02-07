#pragma once

#include "Network/PacketSession.h"

class CAISession :
    public CPacketSession
{
public:
	CAISession();
	~CAISession();

public:
	virtual bool Begin();
	virtual bool End();
	void Update(float fTime);
};

