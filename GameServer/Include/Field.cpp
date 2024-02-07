#include "Field.h"
#include "User.h"
//#include "Stream.h"

CField::CField()
{
}

CField::CField(const CField& field)
{
    *this = field;
}

CField::~CField()
{
}

bool CField::Init()
{
    return true;
}

void CField::Update(float fTime)
{
    CAreaBase::Update(fTime);
}
