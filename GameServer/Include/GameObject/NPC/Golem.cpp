#include "Golem.h"
#include "../../AI/AIDefault.h"

CGolem::CGolem()
{
    m_eMonsterType = MT_Golem;

    m_fAttackEnableTime = 1.286f;
    m_fAttackEnd = 3.17f;
    m_bAttackEnable = true;
}

CGolem::CGolem(const CGolem& monster)
{
    *this = monster;
}

CGolem::~CGolem()
{
}

bool CGolem::Init()
{
    if (!CMonster::Init())
        return false;

    m_pAI = CreateAI<CAIDefault>("Golem");

    m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
    m_pAI->SetAttackEnd(m_fAttackEnd);

    return true;
}

bool CGolem::Init(FILE* pFile)
{
    if (!CMonster::Init(pFile))
        return false;

    m_pAI = CreateAI<CAIDefault>("Golem");

    m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
    m_pAI->SetAttackEnd(m_fAttackEnd);

    return true;
}

void CGolem::Update(float fTime)
{
    CMonster::Update(fTime);
}

CGolem* CGolem::Clone()
{
    return new CGolem(*this);
}

int CGolem::AddPacket(BYTE* pPacket, int iOffset)
{
    int iSize = CMonster::AddPacket(pPacket, iOffset);

    return iSize;
}

void CGolem::Copy(CMonster* Monster)
{
    CMonster::Copy(Monster);
}
