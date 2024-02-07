#include "Timer.h"

CTimer::CTimer() :
	m_fDeltaTime(0.f)
{
}

CTimer::~CTimer()
{
}

bool CTimer::Init()
{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tTime);

	return true;
}

void CTimer::Update()
{
	LARGE_INTEGER	tTime;
	QueryPerformanceCounter(&tTime);

	m_fDeltaTime = (tTime.QuadPart - m_tTime.QuadPart) / (float)m_tSecond.QuadPart;
	m_tTime = tTime;
}
