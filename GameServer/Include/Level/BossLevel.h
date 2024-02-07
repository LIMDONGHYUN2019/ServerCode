#pragma once
#include "Level.h"
class CBossLevel :
    public CLevel
{
	friend class CLevelManager;

private:
	CBossLevel();
	virtual ~CBossLevel();

private:
	int	m_iMonsterNumber;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};

