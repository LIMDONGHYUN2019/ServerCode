#include "User.h"

CUser::CUser()
{
	m_iUserNumber = INT_MAX;
	memset(m_CharacterName, 0, sizeof(m_CharacterName));
	m_Connect = false;
}

CUser::~CUser()
{
}
