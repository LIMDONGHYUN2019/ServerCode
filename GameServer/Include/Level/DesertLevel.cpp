#include "DesertLevel.h"
#include "PathManager.h"
#include "../GameObject/NPC/FinalBoss.h"
#include "../GameObject/NPC/MonsterSpawnPoint.h"

CDesertevel::CDesertevel()
{
	m_LevelType = ELevel::DesertLevel;
}

CDesertevel::~CDesertevel()
{
}

bool CDesertevel::Init()
{
	LoadMonster("DesertMonster.mif");

	// ���� ������ ���缭 ������ �ּ�, �ִ� ���� �����ؾ� �Ѵ�.
	CreateArea(20, 20, Vector3(-2000.f, -2400.f, 0.f), Vector3(6000.f, 2400.f, 0.f));

	return true;
}

void CDesertevel::Update(float DeltaTime)
{
	CLevel::Update(DeltaTime);
}
