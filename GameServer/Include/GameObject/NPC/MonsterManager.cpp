#include "MonsterManager.h"
#include "PathManager.h"
#include "Minion.h"
#include "Golem.h"
#include "Boss.h"
#include "FinalBoss.h"
#include "Sevarog.h"
#include "Troll.h"

#include "../../User.h"

#include <fstream>
#include <sstream>
#include <istream>


DEFINITION_SINGLE(CMonsterManager)

CMonsterManager::CMonsterManager() :
	m_bLoop(true),
	m_iMonsterNumber(0)

{
	QueryPerformanceFrequency(&m_tSecond);
	QueryPerformanceCounter(&m_tTime);
}

CMonsterManager::~CMonsterManager()
{
	//SAFE_DELETE_VECLIST(m_MonsterList);
	//SAFE_DELETE_VECLIST(m_SpawnPointList);

	SAFE_DELETE_MAP(m_MapPrototype);
}

bool CMonsterManager::Init()
{
	// ���� ���� ������ �о�´�.
	FILE* pFile = nullptr;

	char	strPath[MAX_PATH] = {};

	// bin - data ��θ� ��������
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);

	if (pPath)
		strcpy_s(strPath, pPath);

	strcat_s(strPath, "MonsterInfo.mif");

	fopen_s(&pFile, strPath, "rb");
	string	strName = {};
	if (pFile)
	{
		int	MonsterCount = 0;
		fread(&MonsterCount, 4, 1, pFile);
		// ���� ����ŭ �ݺ��ϸ� ���͸� �����ϰ� ���Ϸκ��� ������ �о������ �Ѵ�.
		// ���� ������ �����ϰ� �ִ� ���� ������ �����س��� ���� �ʿ��ϴٸ� ������ ���� ��ü�� ������
		// ���ο� ���� ��ü�� �����ؼ� ����ϵ��� �Ѵ�.
		// �̴� ���Ͽ��� �Ź� �о���� ������ �����̴�.
		for (int i = 0; i < MonsterCount; ++i)
		{
			int	iNameCount = 0;
			fread(&iNameCount, 4, 1, pFile);
			char Tempo[NAME_SIZE] = {};
			fread(Tempo, 1, iNameCount, pFile);

			int	iMonsterType = 0;
			fread(&iMonsterType, 4, 1, pFile);

			CMonster* pMonster = nullptr;  
			switch ((Monster_Type)iMonsterType)
			{
			case MT_Minion:
				pMonster = new CMinion;
				strName = "Minion";
				break;
			case MT_Golem:
				pMonster = new CGolem;
				strName = "Golem";
				break;
			case MT_Troll:
				pMonster = new CTroll;
				strName = "Troll";
				break;
			case MT_Boss:
				pMonster = new CBoss;
				strName = "Boss";
				break;
			case MT_FinalBoss:
				pMonster = new CFinalBoss;
				strName = "FinalBoss";
				break;
			case MT_Sevarog:
				pMonster = new CSevarog;
				strName = "Sevarog";
				break;
			}
			pMonster->Init(pFile);
			// ������ ���͸� ������Ÿ������ ������ش�.
			m_MapPrototype.insert(make_pair(strName, pMonster));
		}

		fclose(pFile);
	}
////========================================================================
//	{
//		char strPath[MAX_PATH] = {};
//		const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);
//		strcpy_s(strPath, pPath);
//		strcat_s(strPath, "MonsterPrototype.csv");
//
//		ifstream file(strPath);
//
//		if (file.fail())
//			cout << "������ ���������� �ʽ��ϴ�. ���� ������ּ���" << endl;
//
//		file.seekg(sizeof(BYTE) * 3);
//		/*
//			�������� UTF8 csv ������ �����ϸ� UTF8-BOM(���� �տ� 3����Ʈ�� �߰� ��) ���·� ����ȴ�.
//			��� utf8�� ��쿡�� ����Ʈ ���� ��ũ�� �ʿ� ������ MS ��ǰ���� �����ϴ� ut8�������� �ڵ����� ���Եȴ�.
//			�׷��� ������ �̿��� ���� �� csv������ ó�� 3byte�� 0xEF, 0xBB, 0xBF �� ��� bom�� ��Ÿ���� ��ũ��� �����ϰ� �������ִ� ó���� �ʿ��ϴ�.
//		*/
//		while (file.good()) // ������ ���� eof�� ���Ë� ����
//		{
//			stringstream ss;
//			bool inquotes = false;
//			vector<string> row;//relying on RVO
//
//			while (file.good())
//			{
//				vector<string> row = csv_read_row(file, ',');
//
//				if (!row[0].find("#")) //���� csv ���� �ȿ� # ���ڰ� �������
//				{
//					continue; //�׳� �ǳ� �پ��
//				}
//				else //#���ڰ� ���� ���
//				{
//					for (int i = 0, leng = row.size() - 2; i < leng; i++)
//						cout << row[i] << "\t "; //�˸°� ���
//				}
//				cout << endl;
//			}
//		}
//
//		file.close();
//	}
//	
////========================================================================
//	memset(strPath, 0, MAX_PATH);
//	if (pPath)
//		strcpy_s(strPath, pPath);
//	//strcat_s(strPath, "WorldMonster.mif");
//	strcat_s(strPath, "MainLevelMonster.mif");
//
//	fopen_s(&pFile, strPath, "rb");
//
//	if (pFile)
//	{
//		int	NameCount;
//		fread(&NameCount, 4, 1, pFile);
//		char	NavName[32] = {};
//		int		Tempo;
//		fread(&Tempo, 4, 1, pFile);
//		fread(NavName, 1, Tempo, pFile);
//
//		int		iMonsterCount = 0;
//		fread(&iMonsterCount, 4, 1, pFile);
//
//		for (int i = 0; i < iMonsterCount; ++i)
//		{
//			CMonsterSpawnPoint* pPoint = new CMonsterSpawnPoint;
//			m_iMonsterNumber++;
//			pPoint->SetMonsterNumber(m_iMonsterNumber);
//			pPoint->Init(pFile);
//			pPoint->SetNavName(NavName);
//			m_SpawnPointList.push_back(pPoint);
//		}
//
//		fclose(pFile);
//	}
	return true;
}

void CMonsterManager::Exit()
{
	m_bLoop = false;
}

void CMonsterManager::Update(float fDeltaTime)
{
	// ���� ���Ͱ� �ִ��� ������Ʈ
	//auto	iter = m_SpawnPointList.begin();
	//auto	iterEnd = m_SpawnPointList.end();

	//for (; iter != iterEnd; ++iter)
	//{
	//	(*iter)->Update(fDeltaTime);
	//}

	//// ������ ���� ���� ������Ʈ
	//auto	iter1 = m_MonsterList.begin();
	//auto	iter1End = m_MonsterList.end();

	//for (; iter1 != iter1End; ++iter1)
	//{
	//	(*iter1)->Update(fDeltaTime);
	//}
}

void CMonsterManager::Run()
{
	while (m_bLoop)
	{
		Update(m_fDeltaTime);
	}
}
//
//void CMonsterManager::SendMonsterInfo(CUser* pUser)
//{
//	auto	iter1 = m_MonsterList.begin();
//	auto	iter1End = m_MonsterList.end();
//
//	int	iOffset = 0;
//
//	int	iCount = m_MonsterList.size();
//
//	BYTE	packet[MAX_BUFFER_LENGTH] = {};
//
//	memcpy(packet, &iCount, sizeof(int));
//
//	iCount = 0;
//
//	iOffset += sizeof(int);
//
//	int	iMonsterPacketSize = 0;
//
//	// ���⼭ ������ ��Ŷ�� �뷮�� �ʹ� ũ��. ���� 1������ 94����Ʈ�� ������ �Ǵµ� �̰� 131������ ��û ū ���� ������ �ȴ�.
//	// ��Ŷ�� 4084 �ε� 12000�� �Ѱ� ������ ������ ���⼭ ������ �߻��Ѵ�.
//	// �׷��Ƿ� ���͸� 1������ �������� �ƴϸ� 10����? �������� ��� �������� �ؾ� �Ұ����� ���δ�.
//	// �̰� �ָ��� �ذ��ؾ� �Ѵ�.
//	for (; iter1 != iter1End; ++iter1)
//	{
//		if (iOffset + iMonsterPacketSize > MAX_BUFFER_LENGTH)
//		{
//			memcpy(packet, &iCount, sizeof(int));
//
//			pUser->WritePacket(GP_LEVELSTART, packet, iOffset);
//
//			iCount = 0;
//			iOffset = 0;
//
//			iOffset += sizeof(int);
//		}
//
//		iMonsterPacketSize = (*iter1)->AddPacket(packet, iOffset);
//		iOffset += iMonsterPacketSize;
//
//		++iCount;
//	}
//
//	memcpy(packet, &iCount, sizeof(int));
//
//	// ��Ŷ�� ��� ����������� Ŭ���̾�Ʈ�� �����ش�.
//	pUser->WritePacket(GP_LEVELSTART, packet, iOffset);
//}

vector<string> CMonsterManager::csv_read_row(istream& file, char delimiter)
{
	stringstream ss;
	bool inquotes = false;
	vector<string> row;//relying on RVO

	while (file.good())
	{
		char c = file.get();

		if (!inquotes && c == '"')
		{
			inquotes = true;
		}
		else if (inquotes && c == '"')
		{
			if (file.peek() == '"')
			{
				ss << (char)file.get();
			}
			else
			{
				inquotes = false;
			}
		}
		else if (!inquotes && c == delimiter)
		{
			row.push_back(ss.str());
			ss.str("");
		}
		else if (!inquotes && (c == '\r' || c == '\n'))
		{
			if (file.peek() == '\n') { file.get(); }
			row.push_back(ss.str());

			return row;
		}
		else
		{
			ss << c;
		}
	}

	return row;
}

//void CMonsterManager::PlayerAttack(CUser* pUser, int iAttack)
//{
//	auto	iter = m_MonsterList.begin();
//	auto	iterEnd = m_MonsterList.end();
//
//	for (; iter != iterEnd; ++iter)
//	{
//		if ((*iter)->GetTarget() == pUser)
//		{
//			(*iter)->Damage(iAttack);
//			break;
//		}
//	}
//}
//
//CMonster* CMonsterManager::FindMonster(int MonsterNumber)
//{ 
//	// ��¿������ ����Ž���� ����.
//	auto	iter = m_MonsterList.begin();
//	auto	iterEnd = m_MonsterList.end();
//
//	for (; iter != iterEnd; ++iter)
//	{
//		if ((*iter)->GetMonsterNumber() == MonsterNumber)
//			return *iter;
//	}
//
//	return nullptr;
//}
