#pragma once

#include "Monster.h"
class CBoss :
    public CMonster
{
	friend class CMonsterManager;

public:
	CBoss();
	CBoss(const CBoss& monster);
	virtual ~CBoss();

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	//virtual void Update(float fTime);
	virtual CBoss* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CMonster* Monster);
};

