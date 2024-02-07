#pragma once

#include "Monster.h"
#include "MonsterSpawnPoint.h"
#include "ThreadSync.h"

class CMonsterManager
{
private:
	// ���� ���� ������ ������ �ְ� �Ѵ�.
	unordered_map<string, CMonster*>	m_MapPrototype;
	//unordered_map<string, CMonster*>	m_BossRoomPrototype;

	// ���� �ʿ� ��ġ�� ���͵��̴�.
	//list<CMonster*>	m_MonsterList;
	//list<CMonsterSpawnPoint*>	m_SpawnPointList;
	// 1. �׳� �����ͷ� ai�� ���� 2. ���ͽĺ���ȣ�� ����

	int				m_iMonsterNumber;
	bool			m_bLoop;

	LARGE_INTEGER	m_tSecond;
	LARGE_INTEGER	m_tTime;
	float			m_fDeltaTime;
	class CTimer*	m_pTimer;

public:
	bool Init();
	vector<string> csv_read_row(istream& file, char delimiter);
	void Update(float fDeltaTime);
	void Exit();
	void Run();

public:
	//const list<class CMonster*>* GetConnectMonsterList()	const
	//{
	//	return &m_MonsterList;
	//}
	//void SendMonsterInfo(class CUser* pUser);
	//void PlayerAttack(class CUser* pUser, int iAttack);

	//CMonster* FindMonster(int MonsterNumber);

private:
	CMonster* FindMonsterPrototype(const string& strName)
	{
		auto	iter = m_MapPrototype.find(strName);

		if (iter == m_MapPrototype.end())
			return nullptr;

		return iter->second;
	}

public:
	template <typename T>
	T* CreateMonsterPrototype(const string& strName) // 
	{
		CMonster* pMonster = FindMonsterPrototype(strName);

		if (pMonster)
			return nullptr;

		pMonster = new T;

		if (!pMonster->Init())
		{
			SAFE_DELETE(pMonster);
			return nullptr;
		}

		m_MapPrototype.insert(make_pair(strName, pMonster));

		return pMonster;
	}

	CMonster* CreateMonsterClone(const string& strPrototypeName)
	{
		// ������Ÿ���� ã�ƿ´�.
		CMonster* pPrototype = FindMonsterPrototype(strPrototypeName);

		if (!pPrototype)
			return nullptr;

		CMonster* pMonster = pPrototype->Clone();

		return pMonster;
	}

	CMonster* CreateMonsterCopy(const string& strPrototypeName, CMonster* Monster)
	{
		// ������Ÿ���� ã�ƿ´�.
		CMonster* pPrototype = FindMonsterPrototype(strPrototypeName);

		if (!pPrototype)
			return nullptr;

		Monster->Copy(pPrototype);

		return Monster;
	}


	DECLARE_SINGLE(CMonsterManager)
};

