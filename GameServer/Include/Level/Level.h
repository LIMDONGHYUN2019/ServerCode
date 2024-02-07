#pragma once

#include "../GameInfo.h"
#include "../GameObject/NPC/Monster.h"
#include<atomic>

// 몬스터 종류별로 관리하기위한
struct MonsterCheckCount
{
	Monster_Type	MonsterType;
	string			MonsterName;
	int				Count;
	list<class CMonsterSpawnPoint*>	SpawnPointList;

	MonsterCheckCount()
	{
		Count = 0;
	}
};

// 각각의 몬스터마다 가지고있는 정보들.
struct MonsterPool
{
	Monster_Type	MonsterType;
	string			MonsterName;
	CMonster*		MemoryPool;
	int				Count;

	MonsterPool()
	{
	}
};

class CLevel
{
	friend class CLevelManager;

protected:
	CLevel();
	virtual ~CLevel();

protected:
	// 몬스터 , NPC, 아이템 등등을 개별적으로 관리한다.
	list<CGameObject*>				m_ObjList;
	list<CMonster*>					m_MonsterList;
	list<CNPC*>						m_NPCList;

	list<CMonsterSpawnPoint*>		m_MonsterSpawnPointList;

	//atomic< list<class CUser*> >		m_UserList;
	list<class CUser*>				m_UserList;
	string							m_Name;
	int								m_iMonsterNumber;
	ELevel							m_LevelType;

	// 몬스터를 미리 생성해둘 풀을 만들어준다.(오브젝트 풀링)
	MonsterPool*					m_MonsterPool;
	int								m_PoolCount;

protected:
	vector<class CGameArea*>		m_vecArea;
	char							m_OtherSendPacket[MAX_BUFFER_LENGTH];
	int								m_iAreaCountH;	// 가로개수
	int								m_iAreaCountV;	// 세로개수
	Vector3							m_vMin;			// 최소값
	Vector3							m_vMax;			// 최대값
	Vector3							m_vSize;
	Vector3							m_vAreaSize;

public:
	int GetAreaCountH()	const
	{
		return m_iAreaCountH;
	}

	int GetAreaCountV()	const
	{
		return m_iAreaCountV;
	}

	const list<class CUser*>* GetUserList()	const
	{
		return &m_UserList;
	}

	const list<class CGameObject*>* GetObjList() const
	{
		return &m_ObjList;
	}

	ELevel GetLevelType()	const
	{
		return m_LevelType;
	}

	void SetName(const string& Name)
	{	
		m_Name = Name;
	}

public:
	void AddUser(class CUser* pUser);
	bool DeleteUser(class CUser* pUser);
	void AddMonster(class CGameObject* Obj)
	{
		m_ObjList.push_back(Obj);
		m_MonsterList.push_back((CMonster*)Obj);
	}

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);

public:
	const list<class CMonster*>* GetConnectMonsterList()	const;
	void SendMonsterInfo(class CUser* pUser);
	void PlayerAttack(class CUser* pUser, int AttackDamage, int MonsterSerial);
	CMonster* FindMonster(int MonsterNumber);

public:
	// 만들고자 하는 가로, 세로 개수와 만들 영역의 최소값, 최대값을 지정한다.
	bool	CreateArea(int iH, int iV, const Vector3& vAreaMin, const Vector3& vAreaMax);

	int		GetAreaIndexH(const Vector3& vPos);
	int		GetAreaIndexV(const Vector3& vPos);
	int		GetAreaIndex(const Vector3& vPos);
	class   CGameArea* GetArea(const Vector3& vPos);
	class   CGameArea* GetArea(int IndexH, int IndexV);
	void	AddAreaUser(class CUser* pUser);
	void	DeleteAreaUser(class CUser* pUser);
	void	SendMoveStartPacket(class CUser* pUser);
	void	SendMovePacket(class CUser* pUser);
	void	SendMoveEndPacket(class CUser* pUser);
	void	OtherAttack(class CUser* pUser, AttackType tAttack);
	void	OtherAttack(class CUser* pUser, int AttackIndex);
	void	OtherRotation(class CUser* pUser, float Yaw);
	void	MonsterAppear(const char* pMonsterInfo, UINT InfoSize);

protected:
	CMonsterSpawnPoint* CreateSpawnPoint(FILE* pFile, const string& NavName);
	bool LoadMonster(const char* FileName, const std::string& PathName = DATA_PATH);
};

