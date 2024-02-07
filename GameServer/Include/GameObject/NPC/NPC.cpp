
#include "NPC.h"
#include "../../UserManager.h"
#include "../../User.h"
#include "Stream.h"
#include "../../Level/Level.h"
#include "../../GameManager.h"

CNPC::CNPC()	:
	m_iAttack(0),
	m_iArmor(0),
	m_iHP(0),
	m_iHPMax(0),
	m_iMP(0),
	m_iMPMax(0),
	m_iLevel(1),
	m_iExp(0),
	m_iGold(0),
	m_bMove(false),
	m_fMoveSpeed(0.f),
	m_fAttackSpeed(0.f),
	m_fTraceRange(0.f),
	m_fAttackRange(0.f),
	m_pAI(nullptr),
	m_fAttackEnableTime(0.f),
	m_iSerialNumber(-1),
	m_bDetectTargetEnable(false),
	m_DetectType(0)
{
	m_eObjType = GameObject_Type::NPC;
}

CNPC::CNPC(const CNPC& obj)	:
	CGameObject(obj)
{
	*this = obj;

	if (obj.m_pAI)
		m_pAI = obj.m_pAI->Clone();
}

CNPC::~CNPC()
{
	SAFE_DELETE(m_pAI);
}

void CNPC::SetDetectTargetEnable(bool Enable)
{
	m_bDetectTargetEnable = Enable;

	if (m_pAI)
		m_pAI->EnableDetectTarget(Enable);
}

void CNPC::AddDetectTargetType(ETargetDetectType Type)
{
	m_DetectType |= (int)Type;

	if (m_pAI)
		m_pAI->AddDetectTargetType(Type);
}

void CNPC::DeleteDetectTargetType(ETargetDetectType Type)
{
	if (m_DetectType & (int)Type)
		m_DetectType ^= (int)Type;

	if (m_pAI)
		m_pAI->DeleteDetectTargetType(Type);
}

bool CNPC::CheckAggroDest(CGameObject* Dest)
{
	if (!m_pAI)
		return false;

	return m_pAI->CheckAggroDest(Dest);
}

void CNPC::AddAggroDest(CGameObject* Dest)
{
	if (!m_pAI)
		return;

	m_pAI->AddAggroDest(Dest);
}

void CNPC::AddAggroPoint(CGameObject* Dest, int Point)
{
	if (!m_pAI)
		return;

	m_pAI->AddAggroPoint(Dest, Point);
}

bool CNPC::Init()
{
	return CGameObject::Init();
}

bool CNPC::Init(FILE* pFile)
{
	return CGameObject::Init(pFile);
}

void CNPC::Update(float fTime)
{
	CGameObject::Update(fTime);

	if (m_pAI)
		m_pAI->Update(fTime);
}

int CNPC::AddPacket(BYTE* pPacket, int iOffset)
{
	return CGameObject::AddPacket(pPacket, iOffset);
}

void CNPC::Copy(CNPC* Obj)
{
	CGameObject::Copy(Obj);

	m_vPos = Obj->m_vPos;
	m_vRot = Obj->m_vRot;
	m_vScale = Obj->m_vScale;

	m_NavName = Obj->m_NavName;
	m_iAttack = Obj->m_iAttack;
	m_iArmor = Obj->m_iArmor;
	m_iHP = Obj->m_iHP;
	m_iHPMax = Obj->m_iHPMax;
	m_iMP = Obj->m_iMP;
	m_iMPMax = Obj->m_iMPMax;
	m_iLevel = Obj->m_iLevel;
	m_iExp = Obj->m_iExp;
	m_iGold = Obj->m_iGold;
	m_fMoveSpeed = Obj->m_fMoveSpeed;
	m_fAttackSpeed = Obj->m_fAttackSpeed;
	m_fTraceRange = Obj->m_fTraceRange;
	m_fAttackRange = Obj->m_fAttackRange;
	m_fDetectTimeMax = Obj->m_fDetectTimeMax;
	m_bMove = Obj->m_bMove;
	m_bDeath = Obj->m_bDeath;

	m_fAttackEnableTime = Obj->m_fAttackEnableTime;
	m_fAttackEnd = Obj->m_fAttackEnd;
	m_bAttackEnable = Obj->m_bAttackEnable;

	m_fIdleTimeMax = Obj->m_fIdleTimeMax;

	m_fTargetDistance = Obj->m_fTargetDistance;
	m_fTargetTimeMax = Obj->m_fTargetTimeMax;

	m_fTracePacketTimeMax = Obj->m_fTracePacketTimeMax;

	m_fPatrolTimeMax = Obj->m_fPatrolTimeMax;

	if (Obj->m_pAI)
	{
		m_pAI = Obj->m_pAI->Clone();
		m_pAI->SetOwner(this);
	}
}

void CNPC::Damage(int iPlayerAttack)
{
}
