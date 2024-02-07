#include "FinalBoss.h"
#include "../../AI/AIDefault.h"

CFinalBoss::CFinalBoss()
{
	m_eMonsterType = MT_FinalBoss;

	m_fAttackEnableTime = 0.676f;
	m_fAttackEnd = 2.47f;
	m_bAttackEnable = true;
}

CFinalBoss::CFinalBoss(const CFinalBoss& monster)
{
	*this = monster;
}

CFinalBoss::~CFinalBoss()
{
}

bool CFinalBoss::Init()
{
	if (!CMonster::Init())
		return false;

	m_pAI = CreateAI<CAIDefault>("FinalBoss");
	m_pAI->SetAttackEnd(m_fAttackEnd);

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);

	return true;
}

bool CFinalBoss::Init(FILE* pFile)
{
	if (!CMonster::Init(pFile))
		return false;
	
	m_pAI = CreateAI<CAIDefault>("FinalBoss");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

CFinalBoss* CFinalBoss::Clone()
{
	return new CFinalBoss(*this);
}

int CFinalBoss::AddPacket(BYTE* pPacket, int iOffset)
{
	int iSize = CMonster::AddPacket(pPacket, iOffset);
	return iSize;
}

void CFinalBoss::Copy(CMonster* Monster)
{
	CMonster::Copy(Monster);
}
