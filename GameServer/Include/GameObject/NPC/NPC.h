#pragma once

#include "../GameObject.h"
#include "../../AI/AIAgent.h"

class CNPC :
    public CGameObject
{
protected:
	CNPC();
	CNPC(const CNPC& obj);
	virtual ~CNPC();

public:
	string				m_NavName;
	int					m_iAttack;
	int					m_iArmor;
	int					m_iHP;
	int					m_iHPMax;
	int					m_iMP;
	int					m_iMPMax;
	int					m_iLevel;
	int					m_iExp;
	int					m_iGold;
	float				m_fMoveSpeed;
	float				m_fAttackSpeed;
	float				m_fTraceRange;	// 플레이어를 인식할 수 있는 반경
	float				m_fAttackRange;	// 플레이어를 공격하기 위한 거리
	bool				m_bMove;
	bool				m_bDetectTargetEnable;
	TargetDetectType	m_DetectType;

protected:
	CAIAgent*			m_pAI;

	float				m_fDetectTimeMax;

	float				m_fTargetTimeMax;
	float				m_fTracePacketTimeMax;
	float				m_fTargetDistance;

	float				m_fAttackEnableTime;
	float				m_fAttackEnd;
	bool				m_bAttackEnable;

	float				m_fPatrolTimeMax;
	float				m_fIdleTimeMax;

	int					m_iSerialNumber;

public:
	CAIAgent* GetAI()	const
	{
		return m_pAI;
	}

	int GetSerialNumber()	const
	{
		return m_iSerialNumber;
	}

	CGameObject* GetTarget()	const
	{
		if (m_pAI)
			return m_pAI->GetTarget();

		return nullptr;
	}

	float GetMoveSpeed()	const
	{
		return m_fMoveSpeed;
	}

	float GetTraceRange()	const
	{
		return m_fTraceRange;
	}

	float GetAttackRange()	const
	{
		return m_fAttackRange;
	}

public:
	void SetNavName(const string& Name)
	{
		m_NavName = Name;
	}

	void SetAI(CAIAgent* Owner)
	{
		m_pAI = Owner;
	}

	void SetMove(bool bMove)
	{
		m_bMove = bMove;
	}

	void SetSerialNumber(int Number)
	{
		m_iSerialNumber = Number;
	}

	void SetPath(const std::list<Vector3>& PathList)
	{
		if (m_pAI)
			m_pAI->SetPath(PathList);
	}

	void SetDetectTargetEnable(bool Enable);

	void AddDetectTargetType(ETargetDetectType Type);

	void DeleteDetectTargetType(ETargetDetectType Type);

	bool CheckAggroDest(CGameObject* Dest);
	void AddAggroDest(CGameObject* Dest);
	void AddAggroPoint(CGameObject* Dest, int Point);

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	virtual CNPC* Clone() = 0;
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CNPC* Obj);
	virtual void Damage(int iPlayerAttack);

public:
	template <typename T>
	T* CreateAI(const std::string& Name)
	{
		SAFE_DELETE(m_pAI);

		m_pAI = new T;
		m_pAI->SetName(Name);
		m_pAI->SetOwner(this);

		SetAI(m_pAI); // 수정부분

		if (!m_pAI->Init())
		{
			SAFE_DELETE(m_pAI);
			return nullptr;
		}

		return (T*)m_pAI;
	}
};

