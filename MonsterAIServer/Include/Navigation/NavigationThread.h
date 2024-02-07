#pragma once

#include "../AIInfo.h"
#include "Thread.h"
#include "CircleQueue.h"

/*
길찾기 방법
1. 모든 레벨의 길찾기 데이터 정보를 다 가지고 있게 한다.

2. 레벨별 스레드를 만들어서 레벨마다의 데이터를 가지고 있게 한다.
-> 2번은 길찾기를 많이 안해도 되는 레벨의 경우에는 스레드만 차지하게 된다.
무슨 데이터?

이런 방법도 괜찮아보인다.

3. 레벨별로 스레드를 만들긴 하지만 레벨별 스레드를 2 ~ 3개 가량을 만들어서 각 레벨의 몬스터가 길찾기를
요청하면 해당 레벨 스레드에 길찾기를 맡기고 결과를 반환해주는 방식으로 처리를 하는것도 좋아보인다.
-> 레벨이 너무 많으면 AI서버를 3-4개로 증가시킬수도 있다.
*/
class CNavigationThread :
    public CThread
{
public:
	CNavigationThread();
	virtual ~CNavigationThread();

private:
	class CNavigation* m_Nav;
	CCircleQueue<NavWorkData, 1000>	m_WorkQueue;

public:
	void SetNavigationData(class CNavigation* Nav)
	{
		m_Nav = Nav;
	}

	int GetWorkCount()	const
	{
		return m_WorkQueue.size();
	}

public:
	virtual void Run();

public:
	bool FindPath(class CPacketSession* Session, int MonsterNumber, ELevel LevelType, const Vector3& StartPos, const Vector3& EndPos);
};

