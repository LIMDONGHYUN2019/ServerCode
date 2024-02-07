#include "Sevarog.h"
#include "../../Level/Level.h"
#include "../../AI/AISevarog.h"


CSevarog::CSevarog()	:
	m_iSkill1Damage(50),
	m_iSkill2Damage(80),
	m_iSkill1Count(0),
	m_iSKill2Count(0),
	m_fSkill1End(5.f),
	m_fSkill2End(10.f),
	m_SkillTime(0.f),
	m_iUseSkill(0)
{
	m_eMonsterType = MT_Sevarog;

	m_fAttackEnableTime = 0.270f;
	m_fAttackEnd = 1.05f;
	m_bAttackEnable = true;

	//m_fSkill1End = 1.01f;
	//m_fSkill2End = 1.40f;
}

CSevarog::CSevarog(const CSevarog& monster)
{
	*this = monster;
}

CSevarog::~CSevarog()
{
}

bool CSevarog::Init()
{
	if (!CMonster::Init())
		return false;

	m_pAI = CreateAI<CAISevarog>("Sevarog");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

bool CSevarog::Init(FILE* pFile)
{
	if (!CMonster::Init(pFile))
		return false;

	m_pAI = CreateAI<CAISevarog>("Sevarog");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

void CSevarog::Update(float fTime)
{
    CMonster::Update(fTime);
}

CSevarog* CSevarog::Clone()
{
    return new CSevarog(*this);
}

int CSevarog::AddPacket(BYTE* pPacket, int iOffset)
{
    int iSize = CMonster::AddPacket(pPacket, iOffset);
    return iSize;
}

void CSevarog::Copy(CMonster* Monster)
{
	CMonster::Copy(Monster);
}
