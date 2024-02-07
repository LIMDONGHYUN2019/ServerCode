	#pragma once

#include "../AIInfo.h"

class CLevel
{
	friend class CLevelManager;

protected:
	CLevel();
	virtual ~CLevel();

protected:
	string	m_Name;

public:
	void SetName(const string& Name)
	{
		m_Name = Name;
	}

public:
	virtual bool Init();
};

