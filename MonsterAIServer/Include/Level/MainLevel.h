#pragma once
#include "Level.h"
class CMainLevel :
    public CLevel
{
	friend class CLevelManager;

protected:
	CMainLevel();
	virtual ~CMainLevel();
};

