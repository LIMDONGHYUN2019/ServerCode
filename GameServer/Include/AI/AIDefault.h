#pragma once
#include "AIAgent.h"
class CAIDefault :
    public CAIAgent
{
public:
	CAIDefault();
	CAIDefault(const CAIDefault& Agent);
	virtual ~CAIDefault();

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual CAIDefault* Clone()	const;
};

