#pragma once
#include "Monster.h"

class CTroll :
	public CMonster
{
	friend class CMonsterManager;

public:
	CTroll();
	CTroll(const CTroll& monster);
	virtual ~CTroll();

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	//virtual void Update(float fTime);
	virtual CTroll* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CMonster* Monster);
};

