#pragma once
#include "AIAgent.h"
class CAISevarog :
    public CAIAgent
{
public:
	CAISevarog();
	CAISevarog(const CAISevarog& Agent);
	virtual ~CAISevarog();

private:
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
	virtual void Update(float fTime);
	virtual CAISevarog* Clone()	const;
};

