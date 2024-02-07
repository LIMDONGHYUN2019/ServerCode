#pragma once

#include "Level.h"

class CMainLevel :
    public CLevel
{
	friend class CLevelManager;

private:
	CMainLevel();
	virtual ~CMainLevel();

private:

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};

