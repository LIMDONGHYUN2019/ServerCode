#include "MonsterSpawnPoint.h"
#include "MonsterManager.h"
#include "Monster.h"
#include "../../Level/Level.h"
#include "../../AI/AIDefault.h"

//#include "GameAreaManager.h"


CMonsterSpawnPoint::CMonsterSpawnPoint()	:
	m_fSpawnTime(0.f),
	m_fSpawnTimeMax(10.f),
	m_pSpawnMonster(nullptr),
	m_Level(nullptr)
{
}

CMonsterSpawnPoint::~CMonsterSpawnPoint()
{
}

void CMonsterSpawnPoint::MonsterDeath()
{
	m_pSpawnMonster = nullptr;
	m_fSpawnTime = 0.f;
}

bool CMonsterSpawnPoint::Init()
{

	return true;
}

bool CMonsterSpawnPoint::Init(FILE* pFile)
{
	Monster_Type	eMonsterType;

	fread(&eMonsterType, 4, 1, pFile);

	switch (eMonsterType)
	{
	case MT_Minion:
		m_strSpawnMonsterName = "Minion";
		m_eMonsterType = MT_Minion;
		break;
	case MT_Boss:
		m_strSpawnMonsterName = "Boss";
		m_eMonsterType = MT_Boss;
		break;
	case MT_Golem:
		m_strSpawnMonsterName = "Golem";
		m_eMonsterType = MT_Golem;
		break;
	case MT_Troll:
		m_strSpawnMonsterName = "Troll";
		m_eMonsterType = MT_Troll;
		break;
	case MT_FinalBoss:
		m_strSpawnMonsterName = "FinalBoss";
		m_eMonsterType = MT_FinalBoss;
		break;
	case MT_Sevarog:
		m_strSpawnMonsterName = "Sevarog";
		m_eMonsterType = MT_Sevarog;
		break;
	}

	fread(&m_vSpawnPos, 12, 1, pFile);
	fread(&m_vSpawnScale, 12, 1, pFile);
	fread(&m_vSpawnRot, 12, 1, pFile);

	int	PatrolPointCount = 0;

	fread(&PatrolPointCount, 4, 1, pFile);

	for (int i = 0; i < PatrolPointCount; ++i)
	{
		Vector3	PatrolPoint;
		fread(&PatrolPoint, 12, 1, pFile);

		m_vecPatrolPoint.push_back(PatrolPoint);
	}

	return true;
}

void CMonsterSpawnPoint::Update(float fTime) // 몬스터가 죽었으면 몇초후에 다시 생성되게끔.....
{
	if (!m_pSpawnMonster)
	{
		m_fSpawnTime += fTime;

		if (m_fSpawnTime >= m_fSpawnTimeMax)
		{
			m_fSpawnTime = 0.f;

			// 메모리 풀에 이미 메모리가 생성되어 있으므로 해당 메모리 주소를 넘겨주어서
			//  몬스터 정보를 단순히 생성되어 있는 메모리 주소에 복사만 해주고
			// m_pPoolMonster 그 값을 그대로 리턴하여 스폰몬스터로 활용한다.
			// 즉, 풀에 할당된 객체를 사용하는 것이다.
			m_pSpawnMonster = GET_SINGLE(CMonsterManager)->CreateMonsterCopy(m_strSpawnMonsterName, m_pPoolMonster);
			//m_pSpawnMonster = GET_SINGLE(CMonsterManager)->CreateMonsterClone(m_strSpawnMonsterName);
			//m_pSpawnMonster = m_pPoolMonster;

			m_Level->AddMonster(m_pSpawnMonster);

			m_pSpawnMonster->m_pLevel = m_Level;
			m_pSpawnMonster->m_eLevelType = m_Level->GetLevelType();
			m_pSpawnMonster->SetPos(m_vSpawnPos);
			m_pSpawnMonster->SetScale(m_vSpawnScale);
			m_pSpawnMonster->SetRotation(m_vSpawnRot);
			m_pSpawnMonster->SetSerialNumber(m_iMonsterNumber);
			m_pSpawnMonster->SetSpawnPoint(this);
			m_pSpawnMonster->SetNavName(m_strNavName);
			m_pSpawnMonster->SetName(m_strSpawnMonsterName.c_str());
			
			CAIAgent* AI = m_pSpawnMonster->GetAI();

			AI->AddPatrolPoint(m_vSpawnPos);
			for (size_t i = 0; i < m_vecPatrolPoint.size(); ++i)
			{
				AI->AddPatrolPoint(m_vecPatrolPoint[i]);
			}

			//몬스터가 새로 생성되었으므로 현재 접속해있는 클라이언트에게 몬스터가 생성되었다고 알려주면
			//클라이언트는 몬스터를 새로 생성해준다.

			// 생성이라기보단 몬스터가 죽으면 다시 원래 몬스터스폰지역에다 스폰시켜야하므로 그걸 알려주기위해 보내는것.

			int Temp = m_Level->GetUserList()->size();
			if (Temp)
			{
				char	strPacket[MAX_BUFFER_LENGTH] = {};

				int iLength = m_pSpawnMonster->AddPacket((BYTE*)strPacket, 0);

				m_Level->MonsterAppear(strPacket, iLength);

				//GET_SINGLE(CGameAreaManager)->MonsterAppear(strPacket, strSize);
			}
		
		}
	}
}
