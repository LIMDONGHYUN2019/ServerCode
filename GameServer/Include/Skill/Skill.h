#pragma once

#include "../GameInfo.h"

class CSkill
{
public:
	CSkill();
	~CSkill();

private:
	SkillInfo*	m_SkillInfo;
	bool		m_SkillEnable;

	bool		m_UseSkill;
	float		m_fSkillCoolTime;
	int			m_iSkillLevel;

public:
	SkillInfo* GetSkillInfo()	const
	{
		return m_SkillInfo;
	}

	void SetSkillLevel(int Level)
	{
		m_iSkillLevel = Level;
	}

	int GetSkillLevel()	const
	{
		return m_iSkillLevel;
	}

	void AddSkillLevel()
	{
		m_iSkillLevel++;
	}

public:
	void Init(SkillInfo* Info);
	void Update(float Time);
	CSkill* Clone();
};

