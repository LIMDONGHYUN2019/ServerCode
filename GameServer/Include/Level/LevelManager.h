#pragma once

#include "../GameInfo.h"

class CLevelManager
{
private:
	unordered_map<string, class CLevel*>	m_mapLevel;

public:
	bool Init();
	void Update(float DeltaTime);

public:
	const list<class CMonster*>* GetConnectMonsterList(const string& LevelName)	const;
	void SendMonsterInfo(const string& LevelName, class CUser* pUser);

	class CMonster* FindMonster(const string& LevelName, int MonsterNumber);
	class CLevel*	FindLevel(ELevel Type)	const;
	class CLevel*	FindLevel(const string& Name)	const;

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

		m_mapLevel.insert(make_pair(Name, Level));

		return Level;
	}


	DECLARE_SINGLE(CLevelManager)
};

