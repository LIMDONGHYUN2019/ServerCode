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

	// DB�� ������ �����ϴ� ��Ŷ�� ������.
	// CharacterInfo, ��ġ, ũ��, ȸ�� ������ �����Ѵ�.
	// InGame �ȿ� ���� ��쿡�� �Ѵ�.

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

			cout << "Socket ::" << GetSocket() << "�÷��̾ AliveReceive���� ���߽��ϴ�." << endl;

			//SaveInventory(); // ��������Ʈ�� ������ ����Ǹ� �׋� �Ҹ��ڸ� ȣ���� ���ִ� ������ ������������� ����.
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
			// �α����ϰ��� Ŭ�� ����ó���ɋ����� ������ ������ �ִ°����� �ۿ��Ѵ�.
		}
	}                                                      

	else
	{
		m_AliveTime += fTime;

		if (m_AliveTime >= m_AliveTimeMax)
		{
			m_AliveTime -= m_AliveTimeMax;

			// Ȯ�� ��Ŷ�� ������.
			CStream	stream;
			char	strPacket[MAX_BUFFER_LENGTH] = {};
			stream.SetBuffer(strPacket);

			WritePacket(GP_ALIVE, (BYTE*)strPacket, stream.GetLength());

			m_AliveReceive = true;
		}
	}

	// ���� �ð��� �ѹ��� ĳ���� ������ �����Ѵ�.
	// ������ ĳ������ ������ CharacterInfo������ ĳ���� Ʈ������ ������ �����Ѵ�.
	// InGame �ȿ� �������� �Ѵ�. InGame�ȿ� ������ üũ�غ���.
	// ���̺���Ŷ : ����� Ŭ���̾�Ʈ�� ����� ����Ǿ� �ִ��� üũ�Ͽ� ������ ���� ���¶��
	// ���� ���� ��Ű���� �Ѵ�.

	m_AutoSaveTime += fTime;

	if (m_AutoSaveTime >= AUTO_SAVE_TIME)
	{
		m_AutoSaveTime -= AUTO_SAVE_TIME;

		GET_SINGLE(CDatabaseManager)->UpdateTransform(m_iUserNumber, m_iCharacterNumber,
			m_vPos.x, m_vPos.y, m_vPos.z,
			m_vScale.x, m_vScale.y, m_vScale.z,
			m_vRot.x, m_vRot.y, m_vRot.z);

		// ������ ������ ���� ��� DB�� �����͸� �����Ѵ�.
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

		//	// �̵� ���̶�� ���� �ð����� �ѹ��� �̵�����ȭ�� ó���ϴ� ��Ŷ�� ������.
		//	CStream	stream1;B
		//	char	strPacket[MAX_BUFFER_LENGTH] = {};
		//	stream1.SetBuffer(strPacket);

		//	stream1.Write<Vector3>(&m_vPos, sizeof(Vector3));

		//	WritePacket(GP_SYNC, (BYTE*)strPacket, stream1.GetLength());
		//}
	}

	else
		m_MoveSyncTime = 0.f;

	// ���ʸ��� Ȯ���ϴ���, ������ ���ʸ�ŭ ��ٸ�����, ��������� ����̳� ��������
	// game ���������� ���� ����
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
	// ���� ��ġ���� ������ġ�� ���� �󸶳� �̵��ߴ��� �Ǵ��Ѵ�.
	// Ŭ���̾�Ʈ �������� ���� ��ġ - ���� ��ġ�� ���־ �󸶳� ���������� �Ǵ��ϴ� ���̴�.
	Vector3	vMove = Pos - m_vPrevPos;

	float	Distance = vMove.Length();

	float	PacketDistance = m_tCharacterInfo.fMoveSpeed * 0.1f;
	float	PacketDistanceRange = PacketDistance * 0.2f;

	// �̵��ӵ��� 110% �̻��� ���� ����� �߸��� ���̴�.
	// Ŭ���̾�Ʈ ������ ���� ���̴�.
	if (Distance >= PacketDistance * 2.f)
	{
		// �� ��� ����ȭ�� ���� �������� �����ϰ� �ִ� ���� ��ġ + �̵����� Ŭ��� ������.
		Vector3	vClientPos = m_vPrevPos + m_vMoveDir * PacketDistance;

		// �� ��ġ�� ��ũ�� ���� �� �ְ� �����ش�.
		CStream	stream;
		char	strPacket[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(strPacket);

		// ��ġ����
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
