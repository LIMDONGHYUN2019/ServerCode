#pragma once
#include "Level.h"
class CBossLevel :
    public CLevel
{
	friend class CLevelManager;

protected:
	CBossLevel();
	virtual ~CBossLevel();
};

