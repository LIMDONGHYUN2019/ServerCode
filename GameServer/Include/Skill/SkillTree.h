#pragma once

#include "../GameInfo.h"

class CSkillTree
{
public:
	CSkillTree();
	CSkillTree(const CSkillTree& Tree);
	~CSkillTree();

private:
	class CUser* m_Owner;
	std::vector<class CSkill*>	m_vecSkillList;
	int		m_iSkillPoint;

public:
	vector<class CSkill*>* GetSkillList()
	{
		return &m_vecSkillList;
	}

	void SetOwner(class CUser* Owner)
	{
		m_Owner = Owner;
	}

public:
	void AddSkillInfo(SkillInfo* Info);
	ESkillUpMessage SkillUp(int iSkillNumber, int& Index, int& Level);
	void SendSkillList(CUser* pSession);

	CSkillTree* Clone();
	void Save();
	void Load();
};

