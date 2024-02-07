#pragma once
#include "Monster.h"
class CGolem :
    public CMonster
{

    friend class CMonsterManager;

public:
	CGolem();
	CGolem(const CGolem& monster);
	virtual ~CGolem();

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	virtual CGolem* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CMonster* Monster);
};

