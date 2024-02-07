#pragma once
#include "Monster.h"

class CFinalBoss :
    public CMonster
{
	friend class CMonsterManager;

public:
	CFinalBoss();
	CFinalBoss(const CFinalBoss& monster);
	virtual ~CFinalBoss();

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	//virtual void Update(float fTime);
	virtual CFinalBoss* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CMonster* Monster);
};

