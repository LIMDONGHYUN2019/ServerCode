#pragma once

#include "../AIInfo.h"
#include "Thread.h"
#include "CircleQueue.h"

/*
��ã�� ���
1. ��� ������ ��ã�� ������ ������ �� ������ �ְ� �Ѵ�.

2. ������ �����带 ���� ���������� �����͸� ������ �ְ� �Ѵ�.
-> 2���� ��ã�⸦ ���� ���ص� �Ǵ� ������ ��쿡�� �����常 �����ϰ� �ȴ�.
���� ������?

�̷� ����� �����ƺ��δ�.

3. �������� �����带 ����� ������ ������ �����带 2 ~ 3�� ������ ���� �� ������ ���Ͱ� ��ã�⸦
��û�ϸ� �ش� ���� �����忡 ��ã�⸦ �ñ�� ����� ��ȯ���ִ� ������� ó���� �ϴ°͵� ���ƺ��δ�.
-> ������ �ʹ� ������ AI������ 3-4���� ������ų���� �ִ�.
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

