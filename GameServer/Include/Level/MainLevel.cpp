
#include "MainLevel.h"
#include "PathManager.h"
#include "../GameObject/NPC/MonsterSpawnPoint.h"
#include "../GameObject/NPC/Golem.h"
#include "../GameObject/NPC/Minion.h"
#include "../GameObject/NPC/Sevarog.h"
#include "../GameObject/NPC/Troll.h"
#include "../GameObject/NPC/Boss.h"
#include "../GameObject/NPC/FinalBoss.h"

CMainLevel::CMainLevel()
{
	m_LevelType = ELevel::MainLevel;
}

CMainLevel::~CMainLevel()
{
}

bool CMainLevel::Init()
{
	LoadMonster("MainLevelMonster.mif");

	CreateArea(20, 20, Vector3(-105000.f, -105000.f, 0.f), Vector3(105000.f, 105000.f, 0.f));

	return true;
}

void CMainLevel::Update(float DeltaTime)
{
	CLevel::Update(DeltaTime);
}
