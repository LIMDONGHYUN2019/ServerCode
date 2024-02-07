#pragma once
#include "Level.h"


class CDesertevel :
	public CLevel
{
	friend class CLevelManager;

private:
	CDesertevel();
	virtual ~CDesertevel();

private:
	int	m_iMonsterNumber;

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
};