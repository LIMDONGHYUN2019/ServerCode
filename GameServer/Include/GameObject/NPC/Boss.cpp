#include "Boss.h"
#include "../../AI/AIDefault.h"

CBoss::CBoss()
{
	m_eMonsterType = MT_Boss;

	m_fAttackEnableTime = 0.286f;
	m_fAttackEnd = 1.17f;
	m_bAttackEnable = true;
}

CBoss::CBoss(const CBoss& monster)
{
	*this = monster;
}

CBoss::~CBoss()
{
}

bool CBoss::Init()
{
	if (!CMonster::Init())
		return false;

	m_pAI = CreateAI<CAIDefault>("Boss");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

bool CBoss::Init(FILE* pFile)
{
	if (!CMonster::Init(pFile))
		return false;

	m_pAI = CreateAI<CAIDefault>("Boss");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

void CBoss::Update(float fTime)
{
	CMonster::Update(fTime);
}

CBoss* CBoss::Clone()
{
	return new CBoss(*this);
}

int CBoss::AddPacket(BYTE* pPacket, int iOffset)
{
	int iSize = CMonster::AddPacket(pPacket, iOffset);
	return iSize;
}

void CBoss::Copy(CMonster* Monster)
{
	CMonster::Copy(Monster);
}
