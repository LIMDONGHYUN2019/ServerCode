#pragma once
#include "Boss.h"
class CSevarog :
    public CBoss
{
	friend class CMonsterManager;

public:
	CSevarog();
	CSevarog(const CSevarog& monster);
	virtual ~CSevarog();

protected:
	int				m_iSkill1Damage;
	int				m_iSkill2Damage;
	float			m_fSkill1End;
	float			m_fSkill2End;

	int				m_iSkill1Count;
	int				m_iSKill2Count;

	float			m_SkillTime;

	int				m_iUseSkill;

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	virtual CSevarog* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CMonster* Monster);
};
