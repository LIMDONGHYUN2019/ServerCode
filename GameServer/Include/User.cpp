#include "User.h"
#include "Inventory.h"
#include "Quest.h"
#include "Stream.h"
#include "DatabaseManager.h"
#include "Skill/SkillTree.h"
#include "GameManager.h"
#include "UserManager.h"
#include "Level/LevelManager.h"
#include "Level/Level.h"

CUser::CUser() :
	m_iCharacterNumber(-1),
	m_bMove(false),
	m_bConnect(false),
	m_strID{},
	m_iUserNumber(0),
	m_pArea(nullptr),
	m_iAreaIndexH(-1),
	m_iAreaIndexV(-1),
	m_iAreaIndex(-1),
	m_strJob{},
	m_pInventory(nullptr),
	m_OtherPlayerTrade(nullptr),
	m_bDeath(false),
	m_AnimationType(EPlayerAnimationType::Idle),
	m_fViewDir(0.f),
	m_MoveSyncTime(0.f),
	m_MoveSyncTimeMax(0.1f),
	m_AutoSaveTime(0.f),
	m_DataChange(false),
	m_MoveChange(false),
	m_AliveTime(0.f),
	m_AliveTimeMax(5.f),
	m_AliveReceiveTime(0.f),
	m_AliveReceiveTimeMax(10.f),
	m_AliveReceive(false)

{
	m_eObjType = GameObject_Type::User;
}

CUser::~CUser()
{
	SAFE_DELETE(m_pSkillTree);
	SAFE_DELETE(m_pInventory);

	for (size_t i = 0; i < m_GroupList.size(); ++i)
	{
		SAFE_DELETE(m_GroupList[i]);
	}
}
void CUser::SetSkillTree(class CSkillTree* SkillTree)
{
	m_pSkillTree = SkillTree;

	if (m_pSkillTree)
	{
		m_pSkillTree->SetOwner(this);
	}
}

bool CUser::Begin()
{
	if (!CPacketSession::Begin())
		return false;
	if (m_tCharacterInfo.iHP <= 0)
		m_bDeath = true;

	return true;
}

bool CUser::End()
{
	if (!CPacketSession::End())
		return false;

	// DB에 정보를 저장하는 패킷을 보낸다.
	// CharacterInfo, 위치, 크기, 회전 정보를 저장한다.
	// InGame 안에 들어갔을 경우에만 한다.

	GET_SINGLE(CDatabaseManager)->UpdateTransform(m_iUserNumber, m_iCharacterNumber, m_vPos.x, m_vPos.y, m_vPos.z, m_vScale.x, m_vScale.y, m_vScale.z,
		m_vRot.x, m_vRot.y, m_vRot.z);
	SaveInventory();
	UpdateCharacterInfo();

	m_pInventory = nullptr;

	m_iCharacterNumber = 0;
	m_bMove = false;
	m_bConnect = false;
	m_iUserNumber = 0;
	m_iAreaIndexH = -1;
	m_iAreaIndexV = -1;
	m_iAreaIndex = -1;
	memset(m_strID, 0, ID_SIZE);
	memset(m_strJob, 0, 128);

	m_vScale = Vector3(0.f, 0.f, 0.f);
	m_vRot = Vector3(0.f, 0.f, 0.f);
	m_vPos = Vector3(0.f, 0.f, 0.f);
	m_vMoveDir = Vector3(0.f, 0.f, 0.f);

	return true;
}

void CUser::Update(float fTime)
{
	if (m_AliveReceive)
	{
		m_AliveReceiveTime += fTime;

		if (m_AliveReceiveTime >= m_AliveReceiveTimeMax)
		{
			m_AliveReceiveTime = 0.f;

			m_AliveReceive = false;

			cout << "Socket ::" << GetSocket() << "플레이어가 AliveReceive하지 못했습니다." << endl;

			//SaveInventory(); // 유저리스트가 서버가 종료되면 그떄 소멸자를 호출을 해주니 유저가 연결끊을떄마다 저장.
			/*SetConnect(false);
			CLevel* Level = CLevelManager::GetInst()->FindLevel(GetLevelType());
			if (Level)
			{
				Level->DeleteUser(this);
			}
			End();
			GET_SINGLE(CUserManager)->DeleteConnectUser(this);
			if (!Begin()) 
			{
				End();
				return;
			}
			CNetSession* Listen = CGameManager::GetInst()->GetListenSession();
			if (!Accept(Listen->GetSocket()))
			{
				End();
				return;
			}*/
			// 로그인하고나서 클라가 정상처리될떄까지 서버는 유저가 있는것으로 작용한다.
		}
	}                                                      

	else
	{
		m_AliveTime += fTime;

		if (m_AliveTime >= m_AliveTimeMax)
		{
			m_AliveTime -= m_AliveTimeMax;

			// 확인 패킷을 보낸다.
			CStream	stream;
			char	strPacket[MAX_BUFFER_LENGTH] = {};
			stream.SetBuffer(strPacket);

			WritePacket(GP_ALIVE, (BYTE*)strPacket, stream.GetLength());

			m_AliveReceive = true;
		}
	}

	// 일정 시간에 한번씩 캐릭터 정보를 저장한다.
	// 저장할 캐릭터의 정보는 CharacterInfo정보와 캐릭터 트랜스폼 정보를 저장한다.
	// InGame 안에 들어갔을때만 한다. InGame안에 들어갔는지 체크해보자.
	// 라이브패킷 : 연결된 클라이언트가 제대로 연결되어 있는지 체크하여 연결이 끊긴 상태라면
	// 접속 종료 시키도록 한다.

	m_AutoSaveTime += fTime;

	if (m_AutoSaveTime >= AUTO_SAVE_TIME)
	{
		m_AutoSaveTime -= AUTO_SAVE_TIME;

		GET_SINGLE(CDatabaseManager)->UpdateTransform(m_iUserNumber, m_iCharacterNumber,
			m_vPos.x, m_vPos.y, m_vPos.z,
			m_vScale.x, m_vScale.y, m_vScale.z,
			m_vRot.x, m_vRot.y, m_vRot.z);

		// 데이터 변경이 있을 경우 DB의 데이터를 갱신한다.
		if (m_DataChange)
		{
			UpdateCharacterInfo();

			SaveInventory();                                                                               

			m_DataChange = false;
		}
	}

	if (m_bMove)
	{
		//CSectionSync	sync(&g_MainCrt);

		m_MoveChange = true;

		m_vPos += m_vMoveDir * m_tCharacterInfo.fMoveSpeed * fTime;
		//m_vPos += m_vMoveDir * 1000 * fTime;

		//m_MoveSyncTime += fTime;

		//if (m_MoveSyncTime >= m_MoveSyncTimeMax)
		//{
		//	m_MoveSyncTime -= m_MoveSyncTimeMax;

		//	// 이동 중이라면 일정 시간마다 한번씩 이동동기화를 처리하는 패킷을 보낸다.
		//	CStream	stream1;B
		//	char	strPacket[MAX_BUFFER_LENGTH] = {};
		//	stream1.SetBuffer(strPacket);

		//	stream1.Write<Vector3>(&m_vPos, sizeof(Vector3));

		//	WritePacket(GP_SYNC, (BYTE*)strPacket, stream1.GetLength());
		//}
	}

	else
		m_MoveSyncTime = 0.f;

	// 몇초마다 확인하는지, 보내고 몇초만큼 기다리는지, 응답없을시 몇번이나 보내는지
	// game 프로토콜을 통해 보냄
}

void CUser::SetInventory(class CInventory* pInventory)
{
	if (!m_pInventory)
		m_pInventory = pInventory;
}

void CUser::SetQuest(CQuest* pQuest)
{
	if (!m_pQuest)
		m_pQuest = pQuest;
}

void CUser::SendQuest()
{
	if (m_pQuest)
		m_pQuest->SendQuest(this);
}

void CUser::SaveQuest()
{
	if (m_pQuest)
		m_pQuest->SaveQuest();
}

void CUser::SendInventory()
{
	if (m_pInventory)
		m_pInventory->SendItemList(this);
}

void CUser::SaveInventory()
{
	if (m_pInventory)
		m_pInventory->SaveItem();
}

void CUser::SendSkill()
{
	if (m_pSkillTree)
		m_pSkillTree->SendSkillList(this);
}

void CUser::SaveSkill()
{
	if (m_pSkillTree)
		m_pSkillTree->Save();
}

bool CUser::AddInventory(_tagItemInfo* pItem)
{
	if (m_pInventory)
		return m_pInventory->AddItem(pItem);

	m_DataChange = true;

	return false;
}

bool CUser::PopInventory(_tagItemInfo* pItem)
{
	if (m_pInventory)
		return m_pInventory->AddItem(pItem);

	m_DataChange = true;

	return false;
}

bool CUser::CompareMove(const Vector3& Pos)
{
	// 현재 위치에서 이전위치를 빼서 얼마나 이동했는지 판단한다.
	// 클라이언트 기준으로 현재 위치 - 이전 위치를 해주어서 얼마나 움직였는지 판단하는 것이다.
	Vector3	vMove = Pos - m_vPrevPos;

	float	Distance = vMove.Length();

	float	PacketDistance = m_tCharacterInfo.fMoveSpeed * 0.1f;
	float	PacketDistanceRange = PacketDistance * 0.2f;

	// 이동속도가 110% 이상이 나올 경우라면 잘못된 것이다.
	// 클라이언트 값만을 비교한 것이다.
	if (Distance >= PacketDistance * 2.f)
	{
		// 이 경우 동기화를 위해 서버에서 저장하고 있던 이전 위치 + 이동값을 클라로 보낸다.
		Vector3	vClientPos = m_vPrevPos + m_vMoveDir * PacketDistance;

		// 이 위치로 싱크를 맞출 수 있게 보내준다.
		CStream	stream;
		char	strPacket[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(strPacket);

		// 위치정보
		stream.Write<Vector3>(&vClientPos, sizeof(Vector3));

		WritePacket(GP_SYNC, (BYTE*)strPacket, stream.GetLength());

		m_vPrevPos = vClientPos;
		m_vPos = vClientPos;

		return false;
	}

	m_vPrevPos = Pos;
	m_vPos = Pos;


	return true;
}

void CUser::UpdateCharacterInfo()
{
	CharacterInfoTable	CharInfo;

	CharInfo.iUserNumber = m_iUserNumber;
	CharInfo.iCharacterNumber = m_iCharacterNumber;
	CharInfo.iCharacterTableNumber = 0;
	CharInfo.iMapLevel = (int)GetLevelType();

	//memcpy((int*)&CharInfo + 4, &m_tCharacterInfo, sizeof(m_tCharacterInfo));

	CharInfo.iAttack = m_tCharacterInfo.iAttack;
	CharInfo.iArmor = m_tCharacterInfo.iArmor;
	CharInfo.fAttackSpeed = m_tCharacterInfo.fAttackSpeed;
	CharInfo.iHP = m_tCharacterInfo.iHP;
	CharInfo.iHPMax = m_tCharacterInfo.iHPMax;
	CharInfo.iMP = m_tCharacterInfo.iMP;
	CharInfo.iMPMax = m_tCharacterInfo.iMPMax;
	CharInfo.fCritical = m_tCharacterInfo.fCritical;
	CharInfo.fCriticalDamage = m_tCharacterInfo.fCriticalDamage;
	CharInfo.fMoveSpeed = m_tCharacterInfo.fMoveSpeed;
	CharInfo.iExp = m_tCharacterInfo.iExp;
	CharInfo.iExpMax = m_tCharacterInfo.iExpMax;
	CharInfo.iMoney = m_tCharacterInfo.iMoney;
	CharInfo.iLevel = m_tCharacterInfo.iLevel;
	CharInfo.iSkillPoint = m_tCharacterInfo.iSkillPoint;
	memcpy(CharInfo.strJob, m_tCharacterInfo.strJob, JOBNAME_SIZE);
	memcpy(CharInfo.strName, m_tCharacterInfo.strName, NAME_SIZE);

	GET_SINGLE(CDatabaseManager)->UpdateCharacterInfo(CharInfo);
}
