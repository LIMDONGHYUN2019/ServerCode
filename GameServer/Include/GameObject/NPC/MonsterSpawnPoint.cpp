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

void CMonsterSpawnPoint::Update(float fTime) // ���Ͱ� �׾����� �����Ŀ� �ٽ� �����ǰԲ�.....
{
	if (!m_pSpawnMonster)
	{
		m_fSpawnTime += fTime;

		if (m_fSpawnTime >= m_fSpawnTimeMax)
		{
			m_fSpawnTime = 0.f;

			// �޸� Ǯ�� �̹� �޸𸮰� �����Ǿ� �����Ƿ� �ش� �޸� �ּҸ� �Ѱ��־
			//  ���� ������ �ܼ��� �����Ǿ� �ִ� �޸� �ּҿ� ���縸 ���ְ�
			// m_pPoolMonster �� ���� �״�� �����Ͽ� �������ͷ� Ȱ���Ѵ�.
			// ��, Ǯ�� �Ҵ�� ��ü�� ����ϴ� ���̴�.
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

			//���Ͱ� ���� �����Ǿ����Ƿ� ���� �������ִ� Ŭ���̾�Ʈ���� ���Ͱ� �����Ǿ��ٰ� �˷��ָ�
			//Ŭ���̾�Ʈ�� ���͸� ���� �������ش�.

			// �����̶�⺸�� ���Ͱ� ������ �ٽ� ���� ���ͽ����������� �������Ѿ��ϹǷ� �װ� �˷��ֱ����� �����°�.

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
