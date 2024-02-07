#pragma once

#include "../GameInfo.h"

class CSkillManager
{
private:
	std::unordered_map<int, SkillInfo*>	m_mapSkillInfo;
	std::vector<class CSkillTree*>	m_vecSkillTree;

	// 직업별 원본 스킬트리를 가지고 있게 한다.
	std::vector<SkillInfo*>	m_vecJobSkillInfo[(int)ESkillJob::End];

public:
	bool Init();
	SkillInfo* FindSkillInfo(int SkillNumber);
	SkillInfo* FindSkillInfo(ESkillType Type, ESkillJob Job, int SerialNumber);
	class CSkillTree* GetSkillTree(ESkillJob Job);

	DECLARE_SINGLE(CSkillManager)
};

