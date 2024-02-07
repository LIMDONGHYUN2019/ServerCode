
#include "LevelManager.h"
#include "Level.h"

DEFINITION_SINGLE(CLevelManager)

CLevelManager::CLevelManager()
{
}

CLevelManager::~CLevelManager()
{
	SAFE_DELETE_VECLIST(m_vecLevel);
}

bool CLevelManager::Init()
{
	return true;
}
