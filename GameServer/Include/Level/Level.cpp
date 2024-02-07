
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

// ó�� ������ ���������� ������ �ѹ� �߰��Ҷ��� ����ϴ� �Լ��̴�.
// ���⿡���� ó�� ������ �߰��� �Ǳ� ������ ���� ������ ���ϰ� �ش� ������
// �ֺ� 8���� ������ ���ؼ� �ش� �����鿡�� �� ������ ���Խ����� �� �ֵ��� �Ѵ�.
void CLevel::AddUser(CUser* pUser)
{
	CSectionSync	sync(&g_MainCrt);
	  
	m_UserList.push_back(pUser);
	pUser->SetLevel(this);

	// ������ �����ش�.
	Vector3	vPos = pUser->GetPos();
	CGameArea* pArea = GetArea(vPos);

	// �� ������ ���� ������ �߽����� 8���� ������ �� ������ �� ������ ���������� �� ���� ��ü�� �����ؼ� ���� �� �ֵ���
	// �˷��ش�. �� 9�� ������ ������ ������ �߰����ش�.
	int	iStartH = pArea->GetIndexH() - 1;
	int	iEndH = pArea->GetIndexH() + 1;

	int	iStartV = pArea->GetIndexV() - 1;
	int	iEndV = pArea->GetIndexV() + 1;

	iStartH = iStartH < 0 ? 0 : iStartH;
	iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;

	iStartV = iStartV < 0 ? 0 : iStartV;
	iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	// ��Ŷ ���۸� �ϳ� ���� ���⿡ ��ó �������� ��� ���θ� �� �Ŀ�
	// ��Ŷ ������ �� ���� ��, ����������..... �̷� ������ ������ �Ѵ�.
	// �� ���� ���� �Ʒ� for���� ��� �Ϸ��ؾ� ���ð��̴�. �׷��Ƿ� �̰��� ��Ŷ���� �����ϴ� ����� �߿��ϴ�.
	memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

	pUser->SetAreaIndex(pArea->GetIndexH(), pArea->GetIndexV(), pArea->GetIndex(), pArea);

	pArea->AddUser(pUser);

	int	UserCount = 0;
	int	OtherPacketLength = 4; // UserCount�� �� �տ�

	for (int i = iStartV; i <= iEndV; ++i)
	{
		for (int j = iStartH; j <= iEndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;

			OtherPacketLength = m_vecArea[iIndex]->UserAppear(&UserCount, pUser, m_OtherSendPacket, OtherPacketLength);
		}
	}
	// ���� �ִ� ������ UserAppear�� ���ش�.
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

	// ������ ���� ������ 8���� ������ ��Ŷ�� ������.
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
	
	// ���� Ŭ�󿡼� DisAppear��  ������ ������ �ִ�(�ߺ�).
	// �ѹ� ���´� ������ �ִ� ������ �ٽ� �Ⱥ����°ɷ�
	// �׷��� ���ʿ� �ٸ������� ������ ������ ������ �����ε�
	// �ᱹ���� 9���������� �� �����ֱ�� �ؾ��Ұ� �����ѵ�
	// �ٸ� ��������� UserDisAppear�� �Ѱ��ָ� ���� ���ſ�� �ƴѰ��ϴ� ������ ����ϴµ�
	// �ƴϸ� ���ʿ� list�� �Լ��� �Ѱ��ִ°ɷδ� ������ ���°��ΰ�...
	// ���� ��� ���� ���� ���������� �Ϲ������� �������� ���ִµ�
	// ���⼭ ������ ������ ������ �ƹ����� ������ ���ٺ��� ���ɻ󿡼� �Ű��� ���̴µ�....
	// �׳� ����ó�� �� �����ٱ�? �ƴϸ� �Լ��� ���� ��ġ�� �ٸ����� ����� ���ڷ� �Ѱ�����ϴ���
	// �׷��� ��Ŷ�� �����°ǵ� �� ����� �̷��� ���������� �װ� �� ���� ���� �ƴѰ�?	
	for (int i = StartV; i <= EndV; ++i)
	{
		for (int j = StartH; j <= EndH; ++j)
		{
			int	iIndex = i * m_iAreaCountH + j;

			OtherPacketLength = m_vecArea[iIndex]->UserDisAppear(&UserCount, pUser, m_OtherSendPacket, OtherPacketLength);
			//m_vecArea[iIndex]->UserDisAppear(pUser);
		}
	}

	// ���� �ִ� ������ UserAppear�� ���ش�.
	if (UserCount > 0)
	{
		memcpy(m_OtherSendPacket, &UserCount, 4);

		pUser->WritePacket(GP_OTHERUSERDISAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

		memset(m_OtherSendPacket, 0, OtherPacketLength);
	}

	// ������ �������� ����.
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
	// 1. ����, ���� ���� ������Ʈ �Ѵ�.
	// 2. ������Ʈ�� ������ ���� �������� ���ŵ��� ���� ���������� �����Ѵ�.
	// 3. ���¿� ���� Ŭ���̾�Ʈ�� ��Ŷ�� �����ش�.
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

			// ���� ��ġ�� ������ ���Ѵ�.
			int iIndex = GetAreaIndex((*iter)->GetPos());

			if (iIndex != -1 && iIndex != (*iter)->GetAreaIndex())
			{
				// ���� ��ġ�� ������ ���Ѵ�.
				int iIndex1 = GetAreaIndex((*iter)->GetPos());
				// ���� 9�� ������ ���Ѵ�.
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

				cout << "Prev Index ���� : " << iPrevIndexH << "Prev Index ���� : " << iPrevIndexV << endl;
				cout << "Current Index ���� : " << iCurIndexH << "Current Index ���� : " << iCurIndexV << endl;

				// ��ġ�� �κа� �Ȱ�ġ�� �κ��� ã�´�.
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

				// ���� �������� �����Ѵ�.
				m_vecArea[PrevIndex]->DeleteUser(*iter);

				memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

				CGameArea* pArea = m_vecArea[iIndex];

				(*iter)->SetAreaIndex(pArea->GetIndexH(), pArea->GetIndexV(), pArea->GetIndex(), pArea);

				Size = vecAppearIndex.size();
				UserCount = 0;
				OtherPacketLength = 4; // UserCount�� �� �տ�

				for (size_t i = 0; i < Size; ++i)
				{
					OtherPacketLength = m_vecArea[vecAppearIndex[i]]->UserAppear(&UserCount, *iter, m_OtherSendPacket, OtherPacketLength);
				}

				// ���� �ִ� ������ UserAppear�� ���ش�.
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

		// Level���� ���� ����
		// Level�̵��� ����

		// 400�� �ϴ� �����µ�
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Update(DeltaTime);
		}
	}

	{
		// ���� ���Ͱ� �ִ��� ������Ʈ
		auto	iter = m_MonsterSpawnPointList.begin();
		auto	iterEnd = m_MonsterSpawnPointList.end();

		for (; iter != iterEnd; ++iter)
		{
			(*iter)->Update(DeltaTime);
		}
	}

	// ������ ����,NPC,�� ���� ������Ʈ
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
	
	// �߻��ߴ� ����
	// ���⼭ ������ ��Ŷ�� �뷮�� �ʹ� ũ��. ���� ���� 1������ 94����Ʈ�� ������ �Ǵµ� �̰� 131������ ��û ū ���� ������ �ȴ�.
	// ��Ŷ�� 4084 �ε� 12000�� �Ѱ� ������ ������ ���⼭ ������ �߻��Ѵ�.
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
	
	// ���� �÷��̾ �����µ� ������ Ÿ���� �����°͸� ������ ������ �Ѵ�.
	// �׷��ٸ� �ٸ� ������ �߰��� �ͼ� Ÿ���� ������ ������ �ȵȴٴ°ǵ�
	// Ȥ�� ���� ������ �ٸ������� �ٸ� ���Ͱ� Trace���̸�
	// �� �� ���� Ÿ���� �÷��̾��̹Ƿ� ���ݿ� ���� �ʾ����� ������ ����� �Ǵ� ������ �ִ�.

	// �������� �ݿ� ���Ѵ� �ϴ��� ���� Ÿ�ٵ鿡�� ���� ������ ����� �ϴµ� �� �����ϋ� �ټ� �������� �����
	// �ȵǴ���? �׷��ٸ� Ÿ���� ��°� ���� ���� 2�������� Ȯ���ϰ�
	// ���⼭ �߿��Ѱ��� MonsterList�� ���� �����ٴ°��̴�.
	// ���� 2�����߿� 19,23���� �ִٰ� �Ѵٸ� ��ó���� 23�� ���� �������� ���͵�
	// 19���� ���� �߰ߵǴ� 19�� �ϰ� break�� �ɸ���. �׷��� 1������ ������ ������ �����̴�.
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
	// ��¿������ ����Ž���� ����.
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
	m_iAreaCountH = iH;// ���� ����
	m_iAreaCountV = iV;// ���� ����
	m_vMin = vAreaMin; // �ּ� ����
	m_vMax = vAreaMax; // �ִ� ������

	m_vSize = vAreaMax - vAreaMin; // ��ü���� ���μ���ũ��

	// Ÿ�� = 20000 / 20 = 1000
	m_vAreaSize.y = m_vSize.y / iH; // ������ ������. ���� / ����, y�� Ÿ�� ũ�� 
	m_vAreaSize.x = m_vSize.x / iV; // x�� Ÿ�� ũ��

	// �� 400���� Ÿ���� ����
	for (int i = 0; i < iV; ++i)
	{
		for (int j = 0; j < iH; ++j)
		{
			CGameArea* pArea = new CGameArea;

			pArea->m_Level = this;

			Vector3	vMin;
			vMin.y = m_vMin.y + j * m_vAreaSize.y;
			vMin.x = m_vMin.x + i * m_vAreaSize.x;

			// �� ���� Ÿ���� �ּҰ��� ��ǻ� ���� Ÿ���� �ִ밪.
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

	//// �� ������ ���� ������ �߽����� 8���� ������ �� ������ �� ������ ���������� �� ���� ��ü�� �����ؼ� ���� �� �ֵ���
	//// �˷��ش�. ���� �ش� Ŭ�󿡼� �� ������ �̹� ���̴� ������ �Ǵ��ϰ� ��ü�� ����� �ξ��ٸ� ������ ����� �۾���
	//// �ǳʶڴ�.
	//int	iStartH = pArea->GetIndexH() - 1;
	//int	iEndH = pArea->GetIndexH() + 1;

	//int	iStartV = pArea->GetIndexV() - 1;
	//int	iEndV = pArea->GetIndexV() + 1;

	//iStartH = iStartH < 0 ? 0 : iStartH;
	//iEndH = iEndH >= m_iAreaCountH ? m_iAreaCountH - 1 : iEndH;

	//iStartV = iStartV < 0 ? 0 : iStartV;
	//iEndV = iEndV >= m_iAreaCountV ? m_iAreaCountV - 1 : iEndV;

	//// ��Ŷ ���۸� �ϳ� ���� ���⿡ ��ó �������� ��� ���θ� �� �Ŀ�
	//// ��Ŷ ������ �� ���� ��, ����������..... �̷� ������ ������ �Ѵ�.
	//// �� ���� ���� �Ʒ� for���� ��� �Ϸ��ؾ� ���ð��̴�. �׷��Ƿ� �̰��� ��Ŷ���� �����ϴ� ����� �߿��ϴ�.
	//memset(m_OtherSendPacket, 0, sizeof(m_OtherSendPacket));

	//int	UserCount = 0;
	//int	OtherPacketLength = 4;

	//for (int i = iStartV; i <= iEndV; ++i)
	//{
	//	for (int j = iStartH; j <= iEndH; ++j)
	//	{
	//		int	iIndex = i * m_iAreaCountH + j;
	//		// ���� ������ ���� �������� ���ο� ������ ������ �� ������ �˸���.
	//		// ��, ���ο� ������ ������ �������� �����ϼ��� �ִ�. �׷��Ƿ� ������ �������� �ִ� ����������
	//		// �Ǵ��ؾ� �Ѵ�. ���Ӱ� ������ ���� / ���� ���̴� ���� �Ǵ� ������ ĳ���� ��ȣ�� �̿��� �Ǵ��Ѵ�.
	//		// �ϴ��� ��� �������� ������ Ŭ���̾�Ʈ���� ���� �������� �ٸ� ���� ����� �ϳ� ������ �ְ�
	//		// ������ �����ϱ� ���� ���� �� ���� ����� ���Ͽ� �ִ��� �Ǵ��Ѵ�.

	//		// �������� ���� : ��ġ, � ĳ�������� ĳ���� ������ ������.
	//		OtherPacketLength = m_vecArea[iIndex]->UserAppear(&UserCount, pUser, m_OtherSendPacket, OtherPacketLength);
	//	}
	//}

	//if (UserCount > 0)
	//{
	//	memcpy(m_OtherSendPacket, &UserCount, 4);

	//	pUser->WritePacket(GP_OTHERUSERAPPEAR, (const BYTE*)m_OtherSendPacket, OtherPacketLength);

	//	memset(m_OtherSendPacket, 0, OtherPacketLength);
	//}
	// �ֺ��� �ִ� �������� ������ ������ �������� �����־ ������ �� �ְ� ���־�� �Ѵ�.
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

	// ��θ� ��������
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

			// ���Ͱ� ������������ �˾Ƴ���.
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
			// �׷��ٸ�  ���⼭ �ش� ���Ͱ� ������ ���Ӱ� �޸� �Ҵ��� �ƴ�
			// ���� ���� ���� �ʱ�ȭ �� ���Ӱ� ����
			// Level�� SpawnPoint�� �������ִ�.
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
