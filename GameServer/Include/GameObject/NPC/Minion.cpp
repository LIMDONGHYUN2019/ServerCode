#include "Minion.h"
#include "../../AI/AIDefault.h"

CMinion::CMinion()
{
    m_eMonsterType = MT_Minion;

    m_fAttackEnableTime = 0.436f;
    m_fAttackEnd = 1.05f;
    //m_fAttackEnd = 3.05f;
    m_bAttackEnable = false;
}

CMinion::CMinion(const CMinion& monster)
{
   *this = monster;
}

CMinion::~CMinion()
{
}

bool CMinion::Init()
{
    if (!CMonster::Init())
        return false;

    m_pAI = CreateAI<CAIDefault>("Minion");

    m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
    m_pAI->SetAttackEnd(m_fAttackEnd);

    return true;
}

bool CMinion::Init(FILE* pFile)
{
    if (!CMonster::Init(pFile))
        return false;

    m_pAI = CreateAI<CAIDefault>("Minion");

    m_pAI->SetAttackEnableTime(m_fAttackEnableTime);
    m_pAI->SetAttackEnd(m_fAttackEnd);

    return true;
}

void CMinion::Update(float fTime)
{
    CMonster::Update(fTime);
}

CMinion* CMinion::Clone()
{
    return new CMinion(*this);
}

int CMinion::AddPacket(BYTE* pPacket, int iOffset)
{
    int iSize = CMonster::AddPacket(pPacket, iOffset);
    return iSize;
}

void CMinion::Copy(CMonster* Monster)
{
    CMonster::Copy(Monster);
}
