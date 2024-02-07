#include "Troll.h"
#include "../../AI/AIDefault.h"

CTroll::CTroll()
{
	m_eMonsterType = MT_Troll;


	m_fAttackEnableTime = 3.086f;
	m_fAttackEnd = 4.37f;
	m_bAttackEnable = true;
}

CTroll::CTroll(const CTroll& monster)
{
	*this = monster;
}

CTroll::~CTroll()
{
}

bool CTroll::Init()
{
	if (!CMonster::Init())
		return false;

	m_pAI = CreateAI<CAIDefault>("Troll");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

bool CTroll::Init(FILE* pFile)
{
	if (!CMonster::Init(pFile))
		return false;

	m_pAI = CreateAI<CAIDefault>("Troll");

	m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
	m_pAI->SetAttackEnd(m_fAttackEnd);

	return true;
}

void CTroll::Update(float fTime)
{
	CMonster::Update(fTime);
}

CTroll* CTroll::Clone()
{
	return new CTroll(*this);
}

int CTroll::AddPacket(BYTE* pPacket, int iOffset)
{
	int iSize = CMonster::AddPacket(pPacket, iOffset);
	return iSize;
}

void CTroll::Copy(CMonster* Monster)
{
	CMonster::Copy(Monster);
}
