#pragma once

#include "GameInfo.h"
#include "Network/PacketSession.h"
#include "GameObject/GameObject.h"

class CUser :
	public CPacketSession, public CGameObject
{
public:
	CUser();
	~CUser();

private://능력치정보
	int		m_iCharacterNumber;
	int		m_iCharacterTableNumber;
	int		m_iUserNumber;
	int		m_iJob;
	_tagCharacterInfo m_tCharacterInfo;

	char	m_strID[ID_SIZE];
	char	m_strJob[128];

	bool	m_bMove;	// 이동중인지 판단한다.
	bool	m_bConnect;
	CUser* m_OtherPlayerTrade;
	//CUser* m_GroupList[GroupUser_Max];
	vector<CUser*> m_GroupList;
	float			m_fViewDir;

	float			m_MoveSyncTime;
	float			m_MoveSyncTimeMax;

	float			m_AutoSaveTime;

	// 체력 변환, 레벨업, 장비 장착
	bool			m_DataChange;
	bool			m_MoveChange;

	class CInventory*	m_pInventory;
	class CSkillTree*	m_pSkillTree;
	class CQuest*		m_pQuest;

private:
	float		m_AliveTime;
	float		m_AliveTimeMax;

	bool		m_AliveReceive;
	float		m_AliveReceiveTime;
	float		m_AliveReceiveTimeMax;

	float		Test = 0;

public:
	void ClearAliveReceive()
	{
		m_AliveReceive = false;
		m_AliveReceiveTime = 0.f;
	}

private: // 영역별
	class CGameArea* m_pArea;
	int		m_iAreaIndexH;
	int		m_iAreaIndexV;
	int		m_iAreaIndex;
	EPlayerAnimationType	m_AnimationType;

protected:
	bool			m_bDeath;
	bool			m_bGroup;

public:
	void SetAnimationType(EPlayerAnimationType Type)
	{
		m_AnimationType = Type;
	}

	void SetSkillTree(class CSkillTree* SkillTree);

	void SetViewDir(float Dir)
	{
		m_fViewDir = Dir;
	}

	void SetDeath(bool bDeath)
	{
		m_bDeath = bDeath;
	}

	void SetGroup(bool Group)
	{
		m_bGroup = Group;
	}

	void SetLevelType(ELevel Type)
	{
		m_tCharacterInfo.MapLevel = Type;
	}

	void SetLevel(CLevel* Level)
	{
		m_pLevel = Level;
	}

	void SetChracterInfo(_tagCharacterInfo* pCharacterInfo)
	{
		memcpy(&m_tCharacterInfo, pCharacterInfo, sizeof(_tagCharacterInfo));
	}

	void SetCharacterNumber(int iNumber)
	{
		m_iCharacterNumber = iNumber;
	}

	void SetAreaIndex(int iH, int iV, int iIndex, class CGameArea* pArea)
	{
		m_iAreaIndexH = iH;
		m_iAreaIndexV = iV;
		m_iAreaIndex = iIndex;
		m_pArea = pArea;
	}

	void SetJobName(const char* pJob)
	{
		strcpy_s(m_strJob, pJob);
	}

	void SetiJob(int iJob)
	{
		m_iJob = iJob;
	}

	void SetConnect(bool bConnect)
	{
		m_bConnect = bConnect;
	}

	void SetInventory(class CInventory* pInventory);

	void SetQuest(class CQuest* pQuest);

	void SetUserNumber(int iUserNumber)
	{
		m_iUserNumber = iUserNumber;
	}

	void SetID(const char* pID)
	{
		strcpy_s(m_strID, pID);
	}
	void SetUserName(const char* pName)
	{
		strcpy_s(m_strName, NAME_SIZE, pName);
	}

	void SetMove(bool bMove)
	{
		m_bMove = bMove;
	}

public:
	EPlayerAnimationType GetAnimationType()	const
	{
		return m_AnimationType;
	}

	ELevel GetLevelType()	const
	{
		return m_tCharacterInfo.MapLevel;
	}

	class CLevel* GetLevel()	const
	{
		return m_pLevel;
	}

	int GetHP()	const
	{
		return m_tCharacterInfo.iHP;
	}

	int GetAttack()	const
	{
		return m_tCharacterInfo.iAttack;
	}

	int GetArmor()	const
	{
		return m_tCharacterInfo.iArmor;
	}

	int GetCharacterNumber()	const
	{
		return m_iCharacterNumber;
	}

	_tagCharacterInfo GetChracterInfo()	const
	{
		return m_tCharacterInfo;
	}

	class CGameArea* GetArea()	const
	{
		return m_pArea;
	}

	const char* GetJobName()	const
	{
		return m_strJob;
	}
	
	const char* GetstrName()	const
	{
		return m_strName;
	}

	const int GetiJob()
	{
		return m_iJob;
	}

	int GetAreaIndexH()	const
	{
		return m_iAreaIndexH;
	}

	int GetAreaIndexV()	const
	{
		return m_iAreaIndexV;
	}

	int GetAreaIndex()	const
	{
		return m_iAreaIndex;
	}

	vector<CUser*>* GetGroupList()
	{
		return &m_GroupList; // 문제
	}

	CUser* GetOtherPlayerTrade()
	{
		return m_OtherPlayerTrade;
	}

	bool GetMove()	const
	{
		return m_bMove;
	}

	bool GetConnect()	const
	{
		return m_bConnect;
	}

	int GetUserNumber()	const
	{
		return m_iUserNumber;
	}

	float GetMoveSpeed() const
	{
		return	m_tCharacterInfo.fMoveSpeed;
	}

	float GetViewDir()	const
	{
		return m_fViewDir;
	}

	CSkillTree* GetSkillTree()
	{
		return m_pSkillTree;
	}

public:
	void AddHP(int iDamage)
	{
		m_tCharacterInfo.iHP -= iDamage;

		if (m_tCharacterInfo.iHP <= 0)
		{
			m_tCharacterInfo.iHP = 0;
			m_bDeath = true;

			// 죽었을 경우에는 바로 갱신한다.
			UpdateCharacterInfo();
			m_DataChange = false;
		}

		else
			m_DataChange = true;
	}

	void AddGroupList(CUser* pSession)
	{
		m_GroupList.push_back(pSession);
	}

	void AddSkillPoint()
	{
		m_tCharacterInfo.iSkillPoint++;
	}

	bool AddInventory(_tagItemInfo* pItem);
	
public:
	bool PopInventory(_tagItemInfo* pItem);


public:
	bool IsDeath()	const
	{
		return m_bDeath;
	}

	bool IsGroup()	const
	{
		return m_bGroup;
	}

	void SubtractSkillPoint()
	{
		m_tCharacterInfo.iSkillPoint--;
	}

	void CloseTrade()
	{
		m_OtherPlayerTrade = nullptr;
	}

	void ClearArea()
	{
		m_pArea = nullptr;
	}

public:
	virtual bool Begin();
	virtual bool End();
	void Update(float fTime);

public:
	void SendInventory();
	void SaveInventory();
	void SendSkill();
	void SaveSkill();
	void SendQuest();
	void SaveQuest();

public:
	virtual bool CompareMove(const Vector3& Pos);

public:
	void UpdateCharacterInfo();
};

