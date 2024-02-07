#pragma once

#include "NPC.h"

class CMonster	:
	public CNPC
{
	friend class CMonsterManager;
	friend class CMonsterSpawnPoint;
	friend class CLevel;

protected:
	CMonster();
	CMonster(const CMonster& monster);
	virtual ~CMonster();

protected:
	class CMonsterSpawnPoint*		m_pSpawnPoint;
	AI_Type							m_MonsterAIType;
	GameObjectProtocol				m_AIProtocol;
	Monster_Type					m_eMonsterType;

public:
	void SetSpawnPoint(class CMonsterSpawnPoint* pPoint)
	{
		m_pSpawnPoint = pPoint;
	}
	CMonsterSpawnPoint* GetSpawnPoint()
	{
		return m_pSpawnPoint;
	}
public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual bool Damage(int iPlayerAttack, class CUser* user);
	virtual bool ItemDrop(class CUser* user);
	virtual void Copy(CMonster* Monster);
	virtual CMonster* Clone() = 0;

protected:
	void TargetTrace(float fTime);
	void Patrol(float fTime);
	void Attack(float fTime);
	void Skill1();
	void Skill2();
};

