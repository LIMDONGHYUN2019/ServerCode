#pragma once
#include "Monster.h"
class CMinion :
    public CMonster
{
	friend class CMonsterManager;

public:
	CMinion();
	CMinion(const CMinion& monster);
	virtual ~CMinion();

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	virtual CMinion* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CMonster* Monster);
};

