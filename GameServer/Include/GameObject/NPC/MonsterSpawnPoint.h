#pragma once

#include "../../GameInfo.h"

class CMonsterSpawnPoint
{
	friend class CLevel;

public:
	CMonsterSpawnPoint();
	~CMonsterSpawnPoint();

private:
	class CLevel*		m_Level;
	class CMonster*		m_pSpawnMonster;
	class CMonster*		m_pPoolMonster;

	Monster_Type		m_eMonsterType;

	string				m_strSpawnMonsterName;
	string				m_strNavName;

	float				m_fSpawnTime;
	float				m_fSpawnTimeMax;

	int					m_iMonsterNumber;

public:
	Vector3				m_vSpawnPos;
	Vector3				m_vSpawnRot;
	Vector3				m_vSpawnScale;
	vector<Vector3>		m_vecPatrolPoint;

public:
	void SetNavName(const char* Name)
	{
		m_strNavName = Name;
	}

	void SetMemoryPoolMonster(class CMonster* Monster)	
	{
		m_pPoolMonster = Monster;
	}

	void SetSpawnMonsterName(const string& Name)
	{
		m_strSpawnMonsterName = Name;
	}

	void SetSpawnTimeMax(float fTime)
	{
		m_fSpawnTimeMax = fTime;
	}
	void SetMonsterNumber(const int& Number)
	{
		m_iMonsterNumber = Number;
	}

public:
	Monster_Type GetCreateMonsterType()	const
	{
		return m_eMonsterType;
	}

	string GetSpawnMonsterName()	const
	{
		return m_strSpawnMonsterName;
	}
	int GetSpawnMonsterNumber()	const
	{
		return m_iMonsterNumber;
	}
	Vector3 GetPos()
	{
		return m_vSpawnPos;
	}
	Vector3 GetRot()
	{
		return m_vSpawnRot;
	}
	Vector3 GetScale()
	{
		return m_vSpawnScale;
	}

	void MonsterDeath();

public:
	bool Init();
	bool Init(FILE* pFile);
	void Update(float fTime);
};

