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
	// 몬스터 정보 파일을 읽어온다.
	FILE* pFile = nullptr;

	char	strPath[MAX_PATH] = {};

	// bin - data 경로를 가져오고
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
		// 몬스터 수만큼 반복하며 몬스터를 생성하고 파일로부터 정보를 읽어오도록 한다.
		// 원본 정보를 저장하고 있는 몬스터 정보를 저장해놓고 추후 필요하다면 리젠시 원본 객체를 복제한
		// 새로운 몬스터 객체를 생성해서 사용하도록 한다.
		// 이는 파일에서 매번 읽어오면 느리기 때문이다.
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
			// 생성한 몬스터를 프로토타입으로 만들어준다.
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
//			cout << "파일이 존재하지가 않습니다. 새로 만들어주세요" << endl;
//
//		file.seekg(sizeof(BYTE) * 3);
//		/*
//			엑셀에서 UTF8 csv 파일을 저장하면 UTF8-BOM(파일 앞에 3바이트가 추가 됨) 형태로 저장된다.
//			사실 utf8의 경우에는 바이트 오더 마크가 필요 없지만 MS 제품에서 생성하는 ut8문서에는 자동으로 삽입된다.
//			그래서 엑셀을 이용해 생성 된 csv파일은 처음 3byte가 0xEF, 0xBB, 0xBF 인 경우 bom을 나타내는 마크라고 생각하고 무시해주는 처리가 필요하다.
//		*/
//		while (file.good()) // 파일의 끝인 eof가 나올떄 까지
//		{
//			stringstream ss;
//			bool inquotes = false;
//			vector<string> row;//relying on RVO
//
//			while (file.good())
//			{
//				vector<string> row = csv_read_row(file, ',');
//
//				if (!row[0].find("#")) //만약 csv 파일 안에 # 문자가 있을경우
//				{
//					continue; //그냥 건너 뛰어라
//				}
//				else //#문자가 없을 경우
//				{
//					for (int i = 0, leng = row.size() - 2; i < leng; i++)
//						cout << row[i] << "\t "; //알맞게 출력
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
	// 죽은 몬스터가 있는지 업데이트
	//auto	iter = m_SpawnPointList.begin();
	//auto	iterEnd = m_SpawnPointList.end();

	//for (; iter != iterEnd; ++iter)
	//{
	//	(*iter)->Update(fDeltaTime);
	//}

	//// 생성된 몬스터 전부 업데이트
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
//	// 여기서 보내는 패킷의 용량이 너무 크다. 몬스터 1마리당 94바이트를 보내게 되는데 이게 131마리면 엄청 큰 값이 나오게 된다.
//	// 패킷은 4084 인데 12000이 넘게 나오기 때문에 여기서 문제가 발생한다.
//	// 그러므로 몬스터를 1마리씩 보내던가 아니면 10마리? 그정도씩 묶어서 보내던가 해야 할것으로 보인다.
//	// 이거 주말에 해결해야 한다.
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
//	// 패킷을 모두 만들어줬으면 클라이언트로 보내준다.
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
//	// 어쩔수없이 선형탐색을 했음.
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
