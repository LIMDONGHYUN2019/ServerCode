#pragma once

#include "../GameInfo.h"

struct AggravationInfo
{
	class CGameObject* Dest;
	int		Point;
	float	Time;
	float	TimeMax;
	
	AggravationInfo() :
		Dest(nullptr),
		Point(0),
		Time(0.f),
		TimeMax(5.f)
	{
	}
};

class CAIAgent
{
public:
	CAIAgent();
	CAIAgent(const CAIAgent& Agent);
	virtual ~CAIAgent();

protected:
	class CNPC*				m_Owner;
	string					m_Name;
	AI_Type					m_PrevAIType;	// ���� �����ӿ� ���� ������ AI
	AI_Type					m_AIType;		// ?
	GameObjectProtocol		m_AIProtocol;

	class CGameObject*		m_pTarget;
	Vector3					m_vTargetPos;
	float					m_fTargetDistance;

	Vector3					m_vBasePos; // �⺻ ��ġ
	float					m_fTraceDistance; // �߰ݰŸ�
	// �ش� ������ ����ġ���� �߰��� �ϴ� �����Ÿ��� �����
	// ����ġ�� ���ư����ϱ� ������ �ƹ����� ����ġ ���� �����ؾ���.
	// ���� �߰��� Ÿ���� ������� �ٷ� ����ġ�� ���°��� �ƴ϶�
	// ��ó�� ���ο� Ÿ������ ������ ��ü�� �ִٸ�? �װ��� Ÿ������ ����������?
	// 

	TargetDetectType		m_DetectType;
	bool					m_bDetectTargetEnable;
	float					m_fDetectTime;
	float					m_fDetectTimeMax;

	float					m_fIdleTime;
	float					m_fIdleTimeMax;

	float					m_TargetTime;
	float					m_TargetTimeMax;

	// Attack
	float					m_fAttackTime; // �ǽð� Ÿ��
	float					m_fAttackEnableTime; // ���� ��� Ÿ��
	float					m_fAttackEnd; // ���� �� Ÿ��
	bool					m_bAttackEnable; // ���� ����
	bool					m_bDamageEnable;

	float					m_fSkill1Time;
	float					m_fSkill1End;
	float					m_fSkill1EnableTime;
	bool					m_bSkill1Enable;

	bool					m_PatrolEnable;
	bool					m_PatrolWaitEnable;
	float					m_fPatrolTime;
	float					m_fPatrolTimeMax;
	float					m_fPatrolWaitTime;
	float					m_fPatrolWaitTimeMax;

	float					m_fTracePacketTime; 
	float					m_fTracePacketTimeMax;

	list<AggravationInfo>	m_AggroList;
	list<Vector3>			m_PathList;

	vector<Vector3>		m_vecPatrolPoint;
	int					m_iPatrolIndex;
	float				m_fPatrolPacketTime;
	float				m_fPatrolPacketTimeMax;

	bool				m_PatrolStart;

public:
	bool CheckAggroDest(CGameObject* Dest)
	{
		auto	iter = m_AggroList.begin();
		auto	iterEnd = m_AggroList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter).Dest == Dest)
				return true;
		}

		return false;
	}

	void AddAggroDest(CGameObject* Dest)
	{
		AggravationInfo	Info;
		Info.Dest = Dest;

		m_AggroList.push_back(Info);
	}

	bool DeleteAggroDest(CGameObject* Dest)
	{
		auto	iter = m_AggroList.begin();
		auto	iterEnd = m_AggroList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter).Dest == Dest)
			{
				m_AggroList.erase(iter);

				return true;
			}
		}

		return false;
	}

	void AddAggroPoint(CGameObject* Dest, int Point)
	{
		auto	iter = m_AggroList.begin();
		auto	iterEnd = m_AggroList.end();

		for (; iter != iterEnd; ++iter)
		{
			if ((*iter).Dest == Dest)
			{
				(*iter).Point += Point;
				(*iter).Time = 0.f;
				return;
			}
		}

		AggravationInfo	Info;
		Info.Dest = Dest;
		Info.Point = Point;

		// ����� ���� ��� �߰��Ѵ�.
		m_AggroList.push_back(Info);
	}

public:
	void SetAttackEnd(float AttackEnd)
	{
		m_fAttackEnd = AttackEnd;
	}

	void SetAttackEnableTime(float AttackEnableTime)
	{
		m_fAttackEnableTime = AttackEnableTime;
	}

	void EnableDetectTarget(bool Target)
	{
		m_bDetectTargetEnable = Target;
	}

	void AddDetectTargetType(ETargetDetectType Type)
	{
		m_DetectType |= (int)Type;
	}

	void DeleteDetectTargetType(ETargetDetectType Type)
	{
		if (m_DetectType & (int)Type)
			m_DetectType ^= (int)Type;
	}

	void SetName(const std::string& Name)
	{
		m_Name = Name;
	}

	void SetOwner(class CNPC* Owner)
	{
		m_Owner = Owner;
	}

	class CNPC* GetOwner()	const
	{
		return m_Owner;
	}

	const std::string& GetName()	const
	{
		return m_Name;
	}

	class CGameObject* GetTarget()	const
	{
		return m_pTarget;
	}

	void SetPath(const list<Vector3>& PathList)
	{
		m_PathList.clear();
		m_PathList = PathList;
	}

	void AddPatrolPoint(const Vector3& Point)
	{
		m_vecPatrolPoint.emplace_back(Point);
	}

public:
	virtual bool Init();
	virtual void Update(float fTime);
	virtual CAIAgent* Clone()	const;

public:
	void TargetTrace(float fTime);
	void ClearTarget();

public:	// Patrol
	void ComputePatrol(float fTime);

protected:
	void Idle();
	void CheckTarget(float fTime);
	void TraceStart();
	void Trace(float fTime);
	void TraceEnd();
	void Attack();
	void Hit(float fTime);
	void Skill1(float fTime);
	void SKill2(float fTime);
	void PatrolStart();
	void Patrol(float fTime);
	void PatrolEnd();
};