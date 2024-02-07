
#include "Skill.h"

CSkill::CSkill()	:
	m_SkillInfo(nullptr),
	m_SkillEnable(false),
	m_UseSkill(true),
	m_fSkillCoolTime(0.f),
	m_iSkillLevel(0)
{
}

CSkill::~CSkill()
{
}

void CSkill::Init(SkillInfo* Info)
{
	m_SkillInfo = Info;
}

void CSkill::Update(float Time)
{
	if (!m_SkillEnable || !m_SkillInfo)
		return;

	if (!m_UseSkill)
	{
		m_fSkillCoolTime += Time;

		if (m_fSkillCoolTime >= m_SkillInfo->fCooldown)
		{
			m_fSkillCoolTime = 0.f;
			m_UseSkill = true;
		}
	}
}

CSkill* CSkill::Clone()
{
	return new CSkill(*this);
}
