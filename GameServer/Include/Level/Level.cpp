
#include "Level.h"
#include "../GameObject/NPC/Monster.h"
#include "../GameObject/NPC/MonsterSpawnPoint.h"
#include "../User.h"
#include "GameArea.h"
#include "PathManager.h"
#include "../GameObject/NPC/Minion.h"
#include "../GameObject/NPC/Boss.h"
#include "../GameObject/NPC/FinalBoss.h"
#include "../GameObject/NPC/Golem.h"
#include "../GameObject/NPC/Sevarog.h"
#include "../GameObject/NPC/Troll.h"

CLevel::CLevel()	:
	m_iMonsterNumber(0),
	m_PoolCount(0),
	m_MonsterPool(nullptr),
	m_OtherSendPacket{}
{
}

CLevel::~CLevel()
{
	for (int i = 0; i < m_PoolCount; ++i)
	{
		SAFE_DELETE_ARRAY(m_MonsterPool[i].MemoryPool);
	}

	SAFE_DELETE_ARRAY(m_MonsterPool);
	SAFE_DELETE_VECLIST(m_vecArea);
	SAFE_DELETE_VECLIST(m_MonsterList);
	SAFE_DELETE_VECLIST(m_MonsterSpawnPointList);
}

// 처음 유저가 접속했을때 레벨에 한번 추가할때만 사용하는 함수이다.
// 여기에서는 처음 레벨에 추가가 되기 때문에 속할 영역을 구하고 해당 영역의
// 주변 8방향 영역을 구해서 해당 영역들에게 이 유저를 포함시켜줄 수 있도록 한다.
void CLevel::AddUser(CUser* pUser)
{
	CSectionSync	sync(&g_MainCrt);
	  
	m_UserList.push_back(pUser);
	pUser->SetLevel(this);

	// 영역을 구해준다.
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);

	// 이 유저가 속한 영역을 중심으로 8방향 영역에 이 유저가 이 영역에 속해있으니 이 유저 객체를 생성해서 보일 수 있도록
	// 알려준다. 총 9개 방행의 영역에 유저를 추가해준다.
	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;

	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;

	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	// 패킷 버퍼를 하나 만들어서 여기에 근처 유저들의 모든 정부를 준 후에
	// 패킷 구조는 총 유저 수, 유저정보들..... 이런 식으로 보내야 한다.
	// 총 유저 수는 아래 for문을 모두 완료해야 나올것이다. 그러므로 이것을 패킷으로 구성하는 방법이 중요하다.
	memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

	pUser->SetAreaIndex(pArea->GetIndexH(), pArea->GetIndexV(), pArea->GetIndex(), pArea);

	pArea->AddUser(pUser);

	int	UserCount = 0;
	int	OtherPacketLength = 4; // UserCount를 맨 앞에

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;

			OtherPacketLength = m_vecArea[iIndex]->UserAppear(&UserCount, pUser, m_OtherSendPacket, OtherPacketLength);
		}
	}
	// 속해 있는 영역에 UserAppear을 해준다.
	if (UserCount > 0)
	{
		memcpy(m_OtherSendPacket, &UserCount, 4);

		pUser->WritePacket(GP_OTHERUSERAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

		memset(m_OtherSendPacket, 0, OtherPacketLength);
	}
}

bool CLevel::DeleteUser(CUser* pUser)
{
	CSectionSync	sync(&g_MainCrt);
	
	m_UserList.remove(pUser);
	//m_ObjList.remove(pUser);

	// 기존에 속한 영역과 8방향 영역에 패킷을 보낸다.
	int	StartH = pUser->GetAreaIndexH() - 1;
	int	StartV = pUser->GetAreaIndexV() - 1;

	int	EndH = pUser->GetAreaIndexH() + 1;
	int	EndV = pUser->GetAreaIndexV() + 1;
	
	StartH = StartH < 0 ? 0 : StartH;
	StartV = StartV < 0 ? 0 : StartV;

	EndH = EndH >= m_iAreaCountH ? m_iAreaCountH - 1 : EndH;
	EndV = EndV >= m_iAreaCountV ? m_iAreaCountV - 1 : EndV;

	memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));
	int	UserCount = 0;
	int	OtherPacketLength = 4;
	
	// 현재 클라에서 DisAppear가  여러번 들어오고 있다(중복).
	// 한번 보냈던 영역에 있는 유저는 다시 안보내는걸로
	// 그런데 애초에 다른유저가 동일한 영역에 있을떄 문제인데
	// 결국에는 9개영역에다 다 보내주기는 해야할거 같긴한데
	// 다른 유저목록을 UserDisAppear로 넘겨주면 많이 무거운거 아닌가하는 생각이 들긴하는데
	// 아니면 애초에 list를 함수에 넘겨주는걸로는 문제가 없는것인가...
	// 예를 들어 마을 시작 스폰지역에 일반적으로 유저들이 모여있는데
	// 여기서 스폰된 유저가 나갈떄 아무래도 유저가 많다보니 성능상에서 신경이 쓰이는데....
	// 그냥 지금처럼 다 보내줄까? 아니면 함수에 같은 위치의 다른유저 목록을 인자로 넘겨줘야하는지
	// 그런데 패킷을 보내는건데 한 사람당 이렇게 보내버리면 그게 더 성능 저하 아닌가?	
	for (int i = StartV; i <= EndV; ++i)
	{
		for (int j = StartH; j <= EndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;

			OtherPacketLength = m_vecArea[iIndex]->UserDisAppear(&UserCount, pUser, m_OtherSendPacket, OtherPacketLength);
			//m_vecArea[iIndex]->UserDisAppear(pUser);
		}
	}

	// 속해 있는 영역에 UserAppear을 해준다.
	if (UserCount > 0)
	{
		memcpy(m_OtherSendPacket, &UserCount, 4);

		pUser->WritePacket(GP_OTHERUSERDISAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

		memset(m_OtherSendPacket, 0, OtherPacketLength);
	}

	// 현재의 영역에서 제거.
	if (pUser->GetArea())
		pUser->GetArea()->DeleteUser(pUser);

	return true;
}

bool CLevel::Init()
{
	return true;
}

void CLevel::Update(float DeltaTime)
{
	// 1. 유저, 몬스터 등을 업데이트 한다.
	// 2. 업데이트된 내용을 토대로 영역별로 제거될지 들어갈지 유지될지를 결정한다.
	// 3. 상태에 따라 클라이언트에 패킷을 보내준다.
	{
		CSectionSync	sync(&g_MainCrt);

		auto	iter = m_UserList.begin();
		auto	iterEnd = m_UserList.end();

		for (; iter != iterEnd;)
		{
			if (!(*iter)->GetConnect())
			{
				iter = m_UserList.erase(iter);
				iterEnd = m_UserList.end();

				continue;
			}

			(*iter)->Update(DeltaTime);

			// 현재 위치의 영역을 구한다.
			int iIndex = GetAreaIndex((*iter)->GetPos());

			if (iIndex != -1 && iIndex != (*iter)->GetAreaIndex())
			{
				// 현재 위치의 영역을 구한다.
				int iIndex1 = GetAreaIndex((*iter)->GetPos());
				// 기존 9개 영역을 구한다.
				list<int>	PrevIndexList;

				int	PrevIndex = (*iter)->GetAreaIndex();
				int iPrevIndexH = (*iter)->GetAreaIndexH();
				int iPrevIndexV = (*iter)->GetAreaIndexV();

				int	PrevMinH = iPrevIndexH - 1 < 0 ? 0 : iPrevIndexH - 1;
				int	PrevMaxH = iPrevIndexH + 1 >= m_iAreaCountH ? m_iAreaCountH - 1 : iPrevIndexH + 1;

				int	PrevMinV = iPrevIndexV - 1 < 0 ? 0 : iPrevIndexV - 1;
				int	PrevMaxV = iPrevIndexV + 1 >= m_iAreaCountV ? m_iAreaCountV - 1 : iPrevIndexV + 1;

				for (int i = PrevMinV; i <= PrevMaxV; ++i)
				{
					for (int j = PrevMinH; j <= PrevMaxH; ++j)
					{
						PrevIndexList.push_back(i * m_iAreaCountH + j);
					}
				}

				int	iCurIndexH = GetAreaIndexH((*iter)->GetPos());
				int	iCurIndexV = GetAreaIndexV((*iter)->GetPos());

				list<int>	CurrentIndexList;

				int	CurrentMinH = iCurIndexH - 1 < 0 ? 0 : iCurIndexH - 1;
				int	CurrentMaxH = iCurIndexH + 1 >= m_iAreaCountH ? m_iAreaCountH - 1 : iCurIndexH + 1;

				int	CurrentMinV = iCurIndexV - 1 < 0 ? 0 : iCurIndexV - 1;
				int	CurrentMaxV = iCurIndexV + 1 >= m_iAreaCountV ? m_iAreaCountV - 1 : iCurIndexV + 1;

				for (int i = CurrentMinV; i <= CurrentMaxV; ++i)
				{
					for (int j = CurrentMinH; j <= CurrentMaxH; ++j)
					{
						CurrentIndexList.push_back(i * m_iAreaCountH + j);
					}
				}

				cout << "Prev Index 가로 : " << iPrevIndexH << "Prev Index 세로 : " << iPrevIndexV << endl;
				cout << "Current Index 가로 : " << iCurIndexH << "Current Index 세로 : " << iCurIndexV << endl;

				// 겹치는 부분과 안겹치는 부분을 찾는다.
				vector<int>	vecDisAppearIndex;
				vector<int>	vecAppearIndex;
				vector<int>	vecPair;

				{
					auto	iterPrev = PrevIndexList.begin();
					auto	iterPrevEnd = PrevIndexList.end();

					for (; iterPrev != iterPrevEnd; ++iterPrev)
					{
						auto	iterCurrent = CurrentIndexList.begin();
						auto	iterCurrentEnd = CurrentIndexList.end();

						for (; iterCurrent != iterCurrentEnd; ++iterCurrent)
						{
							if (*iterPrev == *iterCurrent)
							{
								vecPair.push_back(*iterPrev);
								break;
							}
						}
					}
				}

				size_t	Size = vecPair.size();

				for (size_t i = 0; i < Size; ++i)
				{
					PrevIndexList.remove(vecPair[i]);
					CurrentIndexList.remove(vecPair[i]);
				}

				if (!PrevIndexList.empty())
				{
					std::cout << "DisAppear : ";

					auto	iterPrev = PrevIndexList.begin();
					auto	iterPrevEnd = PrevIndexList.end();

					for (; iterPrev != iterPrevEnd; ++iterPrev)
					{
						std::cout << *iterPrev << ", ";
						vecDisAppearIndex.push_back(*iterPrev);
					}

					std::cout << std::endl;
				}

				if (!CurrentIndexList.empty())
				{
					std::cout << "Appear : ";

					auto	iterCurrent = CurrentIndexList.begin();
					auto	iterCurrentEnd = CurrentIndexList.end();

					for (; iterCurrent != iterCurrentEnd; ++iterCurrent)
					{
						std::cout << *iterCurrent << ", ";
						vecAppearIndex.push_back(*iterCurrent);
					}

					std::cout << std::endl;
				}

				Size = vecDisAppearIndex.size();

				int	UserCount = 0;
				int	OtherPacketLength = 4;

				memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

				for (size_t i = 0; i < Size; ++i)
				{
					//m_vecArea[vecDisAppearIndex[i]]->UserDisAppear(*iter);
					OtherPacketLength = m_vecArea[vecDisAppearIndex[i]]->UserDisAppear(&UserCount, *iter, m_OtherSendPacket, OtherPacketLength);
				}

				if (UserCount > 0)
				{
					memcpy(m_OtherSendPacket, &UserCount, 4);

					(*iter)->WritePacket(GP_OTHERUSERDISAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

					memset(m_OtherSendPacket, 0, OtherPacketLength);
				}

				// 이전 영역에서 제거한다.
				m_vecArea[PrevIndex]->DeleteUser(*iter);

				memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

				CGameArea* pArea = m_vecArea[iIndex];

				(*iter)->SetAreaIndex(pArea->GetIndexH(), pArea->GetIndexV(), pArea->GetIndex(), pArea);

				Size = vecAppearIndex.size();
				UserCount = 0;
				OtherPacketLength = 4; // UserCount를 맨 앞에

				for (size_t i = 0; i < Size; ++i)
				{
					OtherPacketLength = m_vecArea[vecAppearIndex[i]]->UserAppear(&UserCount, *iter, m_OtherSendPacket, OtherPacketLength);
				}

				// 속해 있는 영역에 UserAppear을 해준다.
				if (UserCount > 0)
				{
					memcpy(m_OtherSendPacket, &UserCount, 4);

					(*iter)->WritePacket(GP_OTHERUSERAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

					memset(m_OtherSendPacket, 0, OtherPacketLength);
				}

				pArea->AddUser(*iter);
			}

			++iter;
		}
	}

	{
		auto	iter = m_vecArea.begin();
		auto	iterEnd = m_vecArea.end();

		// Level내에 영역 이전
		// Level이동시 등장

		// 400개 싹다 돌리는데
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Update(DeltaTime);
		}
	}

	{
		// 죽은 몬스터가 있는지 업데이트
		auto	iter = m_MonsterSpawnPointList.begin();
		auto	iterEnd = m_MonsterSpawnPointList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Update(DeltaTime);
		}
	}

	// 생성된 몬스터,NPC,펫 전부 업데이트
	{
		auto	iter1 = m_ObjList.begin();
		auto	iter1End = m_ObjList.end();

		for (; iter1 != iter1End;)
		{
			(*iter1)->Update(DeltaTime);

			if ((*iter1)->IsDeath())
			{

				if ((*iter1)->GetObjType() == GameObject_Type::Monster)
				{
					auto	iter2 = m_MonsterList.begin();
					auto	iter2End = m_MonsterList.end();

					for (; iter2 != iter2End; ++iter2)
					{
						if (*iter1 == *iter2)
						{
							m_MonsterList.erase(iter2);
							break;
						}
					}
				}

				if ((*iter1)->GetObjType() == GameObject_Type::NPC)
				{
					auto	iter2 = m_NPCList.begin();
					auto	iter2End = m_NPCList.end();

					for (; iter2 != iter2End; ++iter2)
					{
						if (*iter1 == *iter2)
						{
							m_NPCList.erase(iter2);
							break;
						}
					}
				}

				else if ((*iter1)->GetObjType() == GameObject_Type::User)
				{
					auto	iter2 = m_UserList.begin();
					auto	iter2End = m_UserList.end();

					for (; iter2 != iter2End; ++iter2)
					{
						if (*iter1 == *iter2)
						{
							m_UserList.erase(iter2);
							break;
						}
					}
				}

				iter1 = m_ObjList.erase(iter1);
				iter1End = m_ObjList.end();
				continue;

			}

			++iter1;
		}
	}
}

const list<class CMonster*>* CLevel::GetConnectMonsterList() const
{
	return &m_MonsterList;
}

void CLevel::SendMonsterInfo(CUser* pUser)
{
	auto	iter1 = m_MonsterList.begin();
	auto	iter1End = m_MonsterList.end();
	
	int	iOffset = 0;
	
	int	iCount = (int)m_MonsterList.size();
	
	BYTE	packet[MAX_BUFFER_LENGTH] = {};
	
	memcpy(packet, &iCount, sizeof(int));
	
	iCount = 0;
	
	iOffset += sizeof(int);
	
	int	iMonsterPacketSize = 0;
	
	// 발생했던 문제
	// 여기서 보내는 패킷의 용량이 너무 크다. 현재 몬스터 1마리당 94바이트를 보내게 되는데 이게 131마리면 엄청 큰 값이 나오게 된다.
	// 패킷은 4084 인데 12000이 넘게 나오기 때문에 여기서 문제가 발생한다.
	for (; iter1 != iter1End; ++iter1)
	{
		if (iOffset + iMonsterPacketSize > MAX_BUFFER_LENGTH)
		{
			memcpy(packet, &iCount, sizeof(int));
	
			pUser->WritePacket(GP_LEVELSTART, packet, iOffset);
	
			iCount = 0;
			iOffset = 0;
	
			iOffset += sizeof(int);
		}
	    
		iMonsterPacketSize = (*iter1)->AddPacket(packet, iOffset);
		iOffset += iMonsterPacketSize;
	
		++iCount;
	}
	memcpy(packet, &iCount, sizeof(int));
	
	pUser->WritePacket(GP_LEVELSTART, packet, iOffset);
}

void CLevel::PlayerAttack(CUser* pUser, int AttackDamage, int MonsterSerial)
{
	auto	iter = m_MonsterList.begin();
	auto	iterEnd = m_MonsterList.end();
	
	// 현재 플레이어가 떄리는데 몬스터의 타겟이 떄리는것만 데미지 적용을 한다.
	// 그렇다면 다른 유저가 중간에 와서 타격은 데미지 적용이 안된다는건데
	// 혹은 현재 전투중 다른곳에서 다른 몬스터가 Trace중이면
	// 이 것 또한 타겟이 플레이어이므로 공격에 맞지 않았지만 데미지 계산이 되는 오류가 있다.

	// 위에것을 반영 안한다 하더라도 지금 타겟들에게 전부 데미지 계산을 하는데 왜 광역일떄 다수 데미지가 계산이
	// 안되는지? 그렇다면 타겟을 잡는게 현재 몬스터 2마리인지 확인하고
	// 여기서 중요한것은 MonsterList를 전부 돌린다는것이다.
	// 만약 2마리중에 19,23번이 있다고 한다면 맨처음에 23번 몬스터 데미지가 들어와도
	// 19번이 먼저 발견되니 19번 하고 break가 걸린다. 그래서 1마리만 데미지 적용이 들어간것이다.
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetSerialNumber() == MonsterSerial)
		{
			if ((*iter)->Damage(AttackDamage, pUser))
			{
				m_MonsterList.erase(iter);
			}
			break;
		}
	}
}

CMonster* CLevel::FindMonster(int MonsterNumber)
{
	// 어쩔수없이 선형탐색을 했음.
	auto	iter = m_MonsterList.begin();
	auto	iterEnd = m_MonsterList.end();

	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetSerialNumber() == MonsterNumber)
			return *iter;
	}

	return nullptr;
}

CMonsterSpawnPoint* CLevel::CreateSpawnPoint(FILE* pFile, const string& NavName)
{
	CMonsterSpawnPoint* pPoint = new CMonsterSpawnPoint;
	m_iMonsterNumber++;
	pPoint->m_Level = this;
	pPoint->SetMonsterNumber(m_iMonsterNumber);
	pPoint->Init(pFile);
	pPoint->SetNavName(NavName.c_str());
	m_MonsterSpawnPointList.push_back(pPoint);

	return pPoint;
}

bool CLevel::CreateArea(int iH, int iV, const Vector3& vAreaMin, const Vector3& vAreaMax)
{
	m_iAreaCountH = iH;// 가로 갯수
	m_iAreaCountV = iV;// 세로 갯수
	m_vMin = vAreaMin; // 최소 왼쪽
	m_vMax = vAreaMax; // 최대 오른쪽

	m_vSize = vAreaMax - vAreaMin; // 전체면적 가로세로크기

	// 타일 = 20000 / 20 = 1000
	m_vAreaSize.y = m_vSize.y / iH; // 갯수로 나눈다. 길이 / 갯수, y축 타일 크기 
	m_vAreaSize.x = m_vSize.x / iV; // x축 타일 크기

	// 총 400개의 타일이 존재
	for (int i = 0; i < iV; ++i)
	{
		for (int j = 0; j < iH; ++j)
		{
			CGameArea* pArea = new CGameArea;

			pArea->m_Level = this;

			Vector3	vMin;
			vMin.y = m_vMin.y + j * m_vAreaSize.y;
			vMin.x = m_vMin.x + i * m_vAreaSize.x;

			// 그 다음 타일의 최소값이 사실상 지금 타일의 최대값.
			Vector3	vMax;
			vMax.y = m_vMin.y + (j + 1) * m_vAreaSize.y;
			vMax.x = m_vMin.x + (i + 1) * m_vAreaSize.x;

			pArea->Init(vMin, vMax, j, i, i * iH + j);

			m_vecArea.push_back(pArea);
		}
	}

	return true;
}

int CLevel::GetAreaIndexH(const Vector3& vPos)
{
	if (vPos.x < m_vMin.x || vPos.x > m_vMax.x)
		return -1;

	return (int)(vPos.x - m_vMin.x) / (int)m_vAreaSize.x;
}

int CLevel::GetAreaIndexV(const Vector3& vPos)
{
	if (vPos.y < m_vMin.y || vPos.y > m_vMax.y)
		return -1;

	return (int)(vPos.y - m_vMin.y) / (int)m_vAreaSize.y;
}

int CLevel::GetAreaIndex(const Vector3& vPos)
{
	int	iH = GetAreaIndexH(vPos);

	if (iH == -1)
		return -1;

	int	iV = GetAreaIndexV(vPos);

	if (iV == -1)
		return -1;

	return iV * m_iAreaCountH + iH;
}

CGameArea* CLevel::GetArea(const Vector3& vPos)
{
	return m_vecArea[GetAreaIndex(vPos)];
}

CGameArea* CLevel::GetArea(int IndexH, int IndexV)
{
	return m_vecArea[IndexV * m_iAreaCountH + IndexH];
}

void CLevel::AddAreaUser(CUser* pUser)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);

	pArea->AddUser(pUser);
	//Vector3	vPos = pUser->GetPos();
	//CGameArea* pArea = GetArea(vPos);

	//CGameArea* pExistingArea = pUser->GetArea();

	////if (pExistingArea == pArea)
	////	return;

	//if (pExistingArea != pArea)
	//	pArea->AddUser(pUser);

	//// 이 유저가 속한 영역을 중심으로 8방향 영역에 이 유저가 이 영역에 속해있으니 이 유저 객체를 생성해서 보일 수 있도록
	//// 알려준다. 만약 해당 클라에서 이 유저를 이미 보이는 유저로 판단하고 객체를 만들어 두었다면 별도로 만드는 작업은
	//// 건너뛴다.
	//int	iStartH = pArea->GetIndexH() - 1;
	//int	iEndH = pArea->GetIndexH() + 1;

	//int	iStartV = pArea->GetIndexV() - 1;
	//int	iEndV = pArea->GetIndexV() + 1;

	//iStartH = iStartH < 0 ? 0 : iStartH;
	//iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;

	//iStartV = iStartV < 0 ? 0 : iStartV;
	//iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	//// 패킷 버퍼를 하나 만들어서 여기에 근처 유저들의 모든 정부를 준 후에
	//// 패킷 구조는 총 유저 수, 유저정보들..... 이런 식으로 보내야 한다.
	//// 총 유저 수는 아래 for문을 모두 완료해야 나올것이다. 그러므로 이것을 패킷으로 구성하는 방법이 중요하다.
	//memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

	//int	UserCount = 0;
	//int	OtherPacketLength = 4;

	//for (int i = iStartV; i <= iEndV; ++i)
	//{
	//	for (int j = iStartH; j <= iEndH; ++j)
	//	{
	//		int	iIndex = i * m_iAreaCountH + j;
	//		// 현재 영역에 속한 유저에게 새로운 유저가 보여질 수 있음을 알린다.
	//		// 단, 새로운 유저는 기존에 보여지는 유저일수도 있다. 그러므로 기존에 보여지고 있는 유저인지를
	//		// 판단해야 한다. 새롭게 보여질 유저 / 원래 보이는 유저 판단 기준은 캐릭터 번호를 이용해 판단한다.
	//		// 일단은 모든 유저에게 보내고 클라이언트에서 현재 보여지는 다른 유저 목록을 하나 가지고 있고
	//		// 유저를 생성하기 전에 먼저 그 유저 목록을 비교하여 있는지 판단한다.

	//		// 보내야할 정보 : 위치, 어떤 캐릭터인지 캐릭터 정보를 보낸다.
	//		OtherPacketLength = m_vecArea[iIndex]->UserAppear(&UserCount, pUser, m_OtherSendPacket, OtherPacketLength);
	//	}
	//}

	//if (UserCount > 0)
	//{
	//	memcpy(m_OtherSendPacket, &UserCount, 4);

	//	pUser->WritePacket(GP_OTHERUSERAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

	//	memset(m_OtherSendPacket, 0, OtherPacketLength);
	//}
	// 주변에 있는 유저들의 정보를 접속한 유저에게 보내주어서 생성할 수 있게 해주어야 한다.
}


void CLevel::DeleteAreaUser(CUser* pUser)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);

	/*CGameArea* pExistingArea = pUser->GetArea();
	if (pExistingArea != pArea)
		pArea->AddUser(pUser);*/

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;

	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;

	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

	int UserCount = 0;
	int UserPacketLength = 4;
	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->UserAppear(&UserCount, pUser, m_OtherSendPacket, UserPacketLength);
		}
	}
}

void CLevel::SendMoveStartPacket(CUser* pUser)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);
	CGameArea* pExistingArea = pUser->GetArea();

	if (pExistingArea != pArea)
		pArea->AddUser(pUser);

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;

	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;

	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->SendMoveStart(pUser);
		}
	}
}

void CLevel::SendMovePacket(CUser* pUser)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);
	CGameArea* pExistingArea = pUser->GetArea();

	if (pExistingArea != pArea)
		pArea->AddUser(pUser);

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;
	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;
	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->SendMove(pUser);
		}
	}
}

void CLevel::SendMoveEndPacket(CUser* pUser)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);
	CGameArea* pExistingArea = pUser->GetArea();

	if (pExistingArea != pArea)
		pArea->AddUser(pUser);

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;
	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;
	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->SendMoveEnd(pUser);
		}
	}
}

void CLevel::OtherAttack(CUser* pUser, AttackType tAttack)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);
	CGameArea* pExistingArea = pUser->GetArea();

	if (pExistingArea != pArea)
		pArea->AddUser(pUser);

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;
	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;
	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->OtherAttack(pUser, tAttack);
		}
	}
}

void CLevel::OtherAttack(CUser* pUser, int AttackIndex)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);
	CGameArea* pExistingArea = pUser->GetArea();

	if (pExistingArea != pArea)
		pArea->AddUser(pUser);

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;
	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;
	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->OtherAttack(pUser, AttackIndex);
		}
	}
}

void CLevel::OtherRotation(CUser* pUser, float Yaw)
{
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);
	CGameArea* pExistingArea = pUser->GetArea();

	if (pExistingArea != pArea)
		pArea->AddUser(pUser);

	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;
	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;
	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;
			m_vecArea[iIndex]->OtherRotation(pUser, Yaw);
		}
	}
}

void CLevel::MonsterAppear(const char* pMonsterInfo, UINT InfoSize)
{
	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->WritePacket(GP_MONSTERCREATE, (BYTE*)pMonsterInfo, InfoSize);
	}
}

bool CLevel::LoadMonster(const char* FileName, const std::string& PathName)
{
	char	strPath[MAX_PATH] = {};

	FILE* pFile = nullptr;

	// 경로를 가져오고
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(PathName);

	if (pPath)
		strcpy_s(strPath, pPath); 

	memset(strPath, 0, MAX_PATH);

	if (pPath)
		strcpy_s(strPath, pPath);
	//strcat_s(strPath, "WorldMonster.mif");
	strcat_s(strPath, FileName);

	fopen_s(&pFile, strPath, "rb");

	if (pFile)
	{
		int	NameCount;
		fread(&NameCount, 4, 1, pFile);
		char	NavName[32] = {};
		int		Tempo;
		fread(&Tempo, 4, 1, pFile);
		fread(NavName, 1, Tempo, pFile);

		int		iMonsterCount = 0;
		fread(&iMonsterCount, 4, 1, pFile);

		list<MonsterCheckCount>	TypeList;

		for (int i = 0; i < iMonsterCount - 4; ++i)
		{
			CMonsterSpawnPoint* pPoint = CreateSpawnPoint(pFile, NavName);

			// 몬스터가 몇종류인지를 알아낸다.
			auto	iter = TypeList.begin();
			auto	iterEnd = TypeList.end();

			bool Pair = false;

			for (; iter != iterEnd; ++iter)
			{
				if ((*iter).MonsterType == pPoint->GetCreateMonsterType())
				{
					(*iter).SpawnPointList.push_back(pPoint);
					++((*iter).Count);
					Pair = true;
					break;
				}
			}

			if (!Pair)
			{
				MonsterCheckCount	Count;
				TypeList.push_back(Count);
				TypeList.back().MonsterType = pPoint->GetCreateMonsterType();
				TypeList.back().Count = 1;
				TypeList.back().MonsterName = pPoint->GetSpawnMonsterName();
				TypeList.back().SpawnPointList.push_back(pPoint);
			}
		}

		m_PoolCount = (int)TypeList.size();

		m_MonsterPool = new MonsterPool[m_PoolCount];
		int	PoolIndex = 0;

		auto	iter = TypeList.begin();
		auto	iterEnd = TypeList.end();

		for (; iter != iterEnd; ++iter, ++PoolIndex)
		{
			// 그렇다면  여기서 해당 몬스터가 죽으면 새롭게 메모리 할당이 아닌
			// 기존 몬스터 스탯 초기화 및 새롭게 스폰
			// Level에 SpawnPoint도 가지고있다.
			m_MonsterPool[PoolIndex].MonsterType = (*iter).MonsterType;
			m_MonsterPool[PoolIndex].Count	= (*iter).Count;
			m_MonsterPool[PoolIndex].MonsterName = (*iter).MonsterName;

			switch ((*iter).MonsterType)
			{
			case MT_Minion:
				m_MonsterPool[PoolIndex].MemoryPool = new CMinion[(*iter).Count];
				break;
			case MT_Golem:
				m_MonsterPool[PoolIndex].MemoryPool = new CGolem[(*iter).Count];
				break;
			case MT_Troll:
				m_MonsterPool[PoolIndex].MemoryPool = new CTroll[(*iter).Count];
				break;
			case MT_Boss:
				m_MonsterPool[PoolIndex].MemoryPool = new CBoss[(*iter).Count];
				break;
			case MT_FinalBoss:
				m_MonsterPool[PoolIndex].MemoryPool = new CFinalBoss[(*iter).Count];
				break;
			case MT_Sevarog:
				m_MonsterPool[PoolIndex].MemoryPool = new CSevarog[(*iter).Count];
				break;
			}

			auto	iter1 = (*iter).SpawnPointList.begin();
			auto	iter1End = (*iter).SpawnPointList.end();

			int		MonsterIndex = 0;

			for (; iter1 != iter1End; ++iter1, ++MonsterIndex)
			{
				m_MonsterPool[PoolIndex].MemoryPool[MonsterIndex].m_pLevel = this;
				m_MonsterPool[PoolIndex].MemoryPool[MonsterIndex].m_eLevelType = m_LevelType;

				(*iter1)->SetMemoryPoolMonster(&m_MonsterPool[PoolIndex].MemoryPool[MonsterIndex]);
			}
		}

		fclose(pFile);
	}

	return true;
}
