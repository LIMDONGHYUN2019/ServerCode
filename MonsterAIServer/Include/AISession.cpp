#include "AISession.h"

CAISession::CAISession()
{
}

CAISession::~CAISession()
{
}

bool CAISession::Begin()
{
	if (!CPacketSession::Begin())
		return false;

    return true;
}

bool CAISession::End()
{
	if (!CPacketSession::End())
		return false;

    return true;
}

void CAISession::Update(float fTime)
{
}
