#pragma once

#include "../AIInfo.h"

class CLevelManager
{
private:
	vector<class CLevel*>	m_vecLevel;

public:
	bool Init();

private:
	template <typename T>
	T* CreateLevel(const string& Name)
	{
		T* Level = new T;

		Level->SetName(Name);

		if (!Level->Init())
		{
			SAFE_DELETE(Level);
			return nullptr;
		}

		m_vecLevel.push_back(Level);

		return Level;
	}

	DECLARE_SINGLE(CLevelManager)
};

