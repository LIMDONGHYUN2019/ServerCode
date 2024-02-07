
#include "AISevarog.h"

CAISevarog::CAISevarog() :
	m_iSkill1Damage(50),
	m_iSkill2Damage(80),
	m_iSkill1Count(0),
	m_iSKill2Count(0),
	m_fSkill1End(5.f),
	m_fSkill2End(10.f),
	m_SkillTime(0.f),
	m_iUseSkill(0)
{
}

CAISevarog::CAISevarog(const CAISevarog& Agent)	:
	CAIAgent(Agent)
{
}

CAISevarog::~CAISevarog()
{
}

bool CAISevarog::Init()
{
	return true;
}

void CAISevarog::Update(float fTime)
{
	CAIAgent::Update(fTime);

	m_SkillTime += fTime;

	// 1�� ��ų
	if (m_iUseSkill == 0)
	{
		if (m_SkillTime >= m_fSkill1End)
		{
			m_SkillTime = 0.f;

			// 1�� ��ų ó��
		}
	}

	// 2�� ��ų
	else if (m_iUseSkill == 1)
	{
		if (m_SkillTime >= m_fSkill2End)
		{
			m_SkillTime = 0.f;

			// 2�� ��ų ó��
		}
	}
}

CAISevarog* CAISevarog::Clone() const
{
	return new CAISevarog(*this);
}
