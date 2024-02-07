
#include "LevelManager.h"
#include "MainLevel.h"
#include "BossLevel.h"

DEFINITION_SINGLE(CLevelManager)

CLevelManager::CLevelManager()
{
}

CLevelManager::~CLevelManager()
{
}

bool CLevelManager::Init()
{
	CreateLevel<CMainLevel>("MainLevel");
	CreateLevel<CBossLevel>("BossLevel");

	return true;
}

void CLevelManager::Update(float DeltaTime)
{
	auto	iter = m_mapLevel.begin();
	auto	iterEnd = m_mapLevel.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->Update(DeltaTime);
	}
}


const list<class CMonster*>* CLevelManager::GetConnectMonsterList(const string& LevelName) const
{
	CLevel* Level = FindLevel(LevelName);

	if (!Level)
		return nullptr;

	return Level->GetConnectMonsterList();
}

void CLevelManager::SendMonsterInfo(const string& LevelName, CUser* pUser)
{
	CLevel* Level = FindLevel(LevelName);

	if (!Level)
		return;

	return Level->SendMonsterInfo(pUser);
}

CMonster* CLevelManager::FindMonster(const string& LevelName, int MonsterNumber)
{
	CLevel* Level = FindLevel(LevelName);

	if (!Level)
		return nullptr;

	return Level->FindMonster(MonsterNumber);
}

CLevel* CLevelManager::FindLevel(ELevel Type) const
{
	auto	iter = m_mapLevel.begin();
	auto	iterEnd = m_mapLevel.end();

	for (; iter != iterEnd; ++iter)
	{
		if (iter->second->GetLevelType() == Type)
			return iter->second;
	}

	return nullptr;
}

CLevel* CLevelManager::FindLevel(const string& Name) const
{
	auto	iter = m_mapLevel.find(Name);

	if (iter == m_mapLevel.end())
		return nullptr;

	return iter->second;
}

