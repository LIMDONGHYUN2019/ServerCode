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
	AI_Type					m_PrevAIType;	// 이전 프레임에 최종 결정된 AI
	AI_Type					m_AIType;		// ?
	GameObjectProtocol		m_AIProtocol;

	class CGameObject*		m_pTarget;
	Vector3					m_vTargetPos;
	float					m_fTargetDistance;

	Vector3					m_vBasePos; // 기본 위치
	float					m_fTraceDistance; // 추격거리
	// 해당 몬스터의 원위치에서 추격을 하다 일정거리를 벗어나면
	// 원위치로 돌아가야하기 떄문에 아무래도 원위치 또한 저장해야함.
	// 만약 추격중 타겟이 사라지면 바로 원위치로 가는것이 아니라
	// 근처에 새로운 타겟으로 지정될 객체가 있다면? 그것을 타겟으로 잡을것인지?
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
	float					m_fAttackTime; // 실시간 타임
	float					m_fAttackEnableTime; // 공격 모션 타임
	float					m_fAttackEnd; // 공격 끝 타임
	bool					m_bAttackEnable; // 공격 가능
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

		// 대상이 없을 경우 추가한다.
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