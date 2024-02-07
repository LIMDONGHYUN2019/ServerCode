
#include "BossLevel.h"
#include "PathManager.h"
#include "../GameObject/NPC/FinalBoss.h"
#include "../GameObject/NPC/Sevarog.h"
#include "../GameObject/NPC/MonsterSpawnPoint.h"

CBossLevel::CBossLevel()
{
	m_LevelType = ELevel::SevarogBossRoom;
}

CBossLevel::~CBossLevel()
{
}

bool CBossLevel::Init()
{
	LoadMonster("SevarogLevelMonster.mif");

	// 보스 레벨에 맞춰서 개수나 최소, 최대 값을 설정해야 한다.
	CreateArea(20, 20, Vector3(-2000.f, -2400.f, 0.f), Vector3(6000.f, 2400.f, 0.f));

	return true;
}

void CBossLevel::Update(float DeltaTime)
{
	CLevel::Update(DeltaTime);
}
