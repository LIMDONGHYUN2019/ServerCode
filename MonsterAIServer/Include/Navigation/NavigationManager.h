#pragma once

#include "../AIInfo.h"

class CNavigationManager
{
private:
	unordered_map<string, class CNavigation*>	m_mapNav;
	unordered_map<string, vector<class CNavigationThread*>>	m_mapNavThread;

public:
	bool Init();
	bool FindPath(class CPacketSession* Session, const string& NavName, int MonsterNumber, ELevel LevelType, const Vector3& StartPos, const Vector3& EndPos);

	class CNavigation* FindNavigation(const string& Name);

private:
	class CNavigationThread* FindNavigationThread(const string& Name);


	DECLARE_SINGLE(CNavigationManager)
};

