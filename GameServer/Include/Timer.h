#pragma once

#include "GameInfo.h"

class CTimer
{
public:
	CTimer();
	~CTimer();

private:
	LARGE_INTEGER	m_tSecond;
	LARGE_INTEGER	m_tTime;
	float			m_fDeltaTime;

public:
	float GetDeltaTime()	const
	{
		return m_fDeltaTime;
	}

public:
	bool Init();
	void Update();
};

