#pragma once

#include "GameInfo.h"
#include "AreaBase.h"

class CField : 
	public CAreaBase
{
public:
	CField();
	CField(const CField& field);
	virtual ~CField();
	

public:
	virtual bool Init();
	virtual void Update(float fTime);
};

