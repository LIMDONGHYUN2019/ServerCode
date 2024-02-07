
#include "GameObject.h"

CGameObject::CGameObject()	:
	m_bDeath(false),
	m_pLevel(nullptr),
	m_eLevelType(ELevel::None),
	m_strName{},
	m_vScale(1.f, 1.f, 1.f)
{
}

CGameObject::CGameObject(const CGameObject& obj)
{
	*this = obj;
}

CGameObject::~CGameObject()
{
}

bool CGameObject::Init()
{
	return true;
}

bool CGameObject::Init(FILE* pFile)
{
	return true;
}

void CGameObject::Update(float fTime)
{
}

CGameObject* CGameObject::Clone()
{
	return nullptr;
}

int CGameObject::AddPacket(BYTE* pPacket, int iOffset)
{
	return 0;
}

void CGameObject::Copy(CGameObject* Obj)
{
}

bool CGameObject::CompareMove(const Vector3& Pos)
{

	return false;
}
