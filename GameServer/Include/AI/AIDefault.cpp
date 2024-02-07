
#include "AIDefault.h"
#include "../GameObject/GameObject.h"
#include "../Level/Level.h"
#include "../GameObject/NPC/MonsterSpawnPoint.h"
#include "../UserManager.h"
#include "../User.h"
#include "Stream.h"
#include "../GameManager.h"
#include "Network/PacketSession.h"

CAIDefault::CAIDefault()
{
}

CAIDefault::CAIDefault(const CAIDefault& Agent)	:
	CAIAgent(Agent)
{
}

CAIDefault::~CAIDefault()
{
}

bool CAIDefault::Init()
{
	return true;
}

void CAIDefault::Update(float fTime)
{
	// 부모의 Update에서 Target만 잡아준다.
	CAIAgent::Update(fTime);

	int	SerialNumber = m_Owner->GetSerialNumber();
	auto pUserList = m_Owner->GetLevel()->GetUserList();
	bool	FindPath = false;
	/*
	패트롤 중에 유저가 접속하면 해당 유저는 PatrolStart를 못받는다.
	그러므로 유저가 접속했을때 몬스터 생성 정보를 넘겨줄때 패트롤 중인지 정보까지 같이 넘겨주어야 한다.
	이는 몬스터가 다른 유저를 쫓고 있을때 Trace 상태도 마찬가지로 넘겨주어야 한다.
	*/

	if (m_pTarget)
	{
		// Trace라면 FindPath까지 보낸다.
		CheckTarget(fTime);
		// Trace에서 벗어났으면 어그로List랑 UserList에서 제외시켜야한다.

		m_PatrolStart = true;
		m_PatrolWaitEnable = true;
		m_iPatrolIndex = 0;
	}

	else
	{
		if (m_vecPatrolPoint.size() > 1)
		{
			m_AIType = AI_Type::Patrol;

			if (m_PatrolWaitEnable)
			{
				m_fPatrolWaitTime += fTime;

				if (m_fPatrolWaitTime >= m_fPatrolWaitTimeMax)
				{
					m_fPatrolWaitTime = 0.f;
					m_PatrolWaitEnable = false;

					m_AIProtocol = GameObjectProtocol::MA_PATROLSTART;
					m_PatrolStart = true;
				}

				else
				{
					m_AIType = AI_Type::Idle;
					m_AIProtocol = GameObjectProtocol::MA_IDLE;
					Idle();
				}
			}

			else
			{
				// Patrol을 진행해야 하는 상황이라면 이동해야할 패트롤 포인트의 위치를 얻어와서
				// 그 위치로 향하는 방향을 구하고 이동을 시켜준다.
				// 단, 패트롤로 처음 바뀔 때에는 이동 방향과 위치를 보내주고 그 다음 프레임부터
				// 이동을 실시한다.
				Vector3	PatrolPos = m_vecPatrolPoint[m_iPatrolIndex];
				Vector3	Pos = m_Owner->GetPos();

				// 일단 플레이어의 Pos는 우리가 설정한 좌표는 캐릭터 가운데이기 떄문에
				// 그에 반해 몬스터는 z가 0이기 떄문에
				// 플레이어와 몬스터의 거리에서 z가 -값이 될수가 있다.
				PatrolPos.z = 0.f;
				Pos.z = 0.f;

				Vector3	Dir = PatrolPos - Pos;
				float	TargetDist = Dir.Length();

				// 이미 도착한 다음에 계산이 일어날 수도 있으므로 거리가 0일 경우 도착으로 처리한다.
				if (TargetDist == 0.f)
				{
					m_AIProtocol = MA_PATROLEND;
					//cout << "TargetDist == 0....." << endl;
					m_iPatrolIndex = (m_iPatrolIndex + 1) % m_vecPatrolPoint.size();
					m_PatrolWaitEnable = true;
					m_Owner->SetMove(false);
				}

				else
				{
					Dir.Normalize();
					float	MoveDist = m_Owner->GetMoveSpeed() * fTime;
					bool	Arrive = false;
					if (MoveDist >= TargetDist)
					{
						MoveDist = TargetDist;
						Arrive = true;
					}

					Pos = m_Owner->GetPos();
					Pos += Dir * MoveDist;
					m_Owner->SetPos(Pos);
					m_Owner->SetMoveDir(Dir);
					m_Owner->SetMove(true);
					Pos.z = m_vecPatrolPoint[m_iPatrolIndex].z;

					if (Arrive)
					{
						m_AIProtocol = MA_PATROLEND;
						m_PatrolWaitEnable = true;
						m_Owner->SetMove(false);
						cout << ((CMonster*)m_Owner)->GetName() << "  " << ((CMonster*)m_Owner)->GetSpawnPoint()->GetSpawnMonsterNumber() <<
							"번 몬스터  " << m_iPatrolIndex << "번 위치에  " << "Patrol Arrive..." << endl;
						m_iPatrolIndex = (m_iPatrolIndex + 1) % m_vecPatrolPoint.size();
					}

					else
					{
						m_AIProtocol = MA_PATROL;
					}
				}
			}
		}

		else // Patrol이 없을떄
		{
			m_AIType = AI_Type::Idle;
			m_AIProtocol = GameObjectProtocol::MA_IDLE;
			Idle();
		}
	}

	switch (m_AIType)
	{
		case AI_Type::Idle:
			m_Owner->m_bMove = false;
			m_bAttackEnable = true;
			m_fAttackTime = 0.f;
			m_fTracePacketTime = 0.f;
			m_fPatrolPacketTime = 0.f;
			m_PathList.clear();

        break;
		case AI_Type::Trace:
		{
			m_bAttackEnable = true;
			m_fAttackTime = 0.f;
			m_fPatrolPacketTime = 0.f;
			TargetTrace(fTime);
	
			switch (m_AIProtocol)
			{
				case MA_TRACESTART:
				{
					TraceStart();
				}
				break;
				case MA_TRACE:
				{
					Trace(fTime);
				}
				break;
				case MA_TRACEEND:
				{
					TraceEnd();
				}
				break;
			}
		}
			break;
		case AI_Type::Attack:
		{
			Attack();
			Hit(fTime);
		}
			break;
		case AI_Type::Death:
		{
			m_bAttackEnable = false;
			m_fAttackTime = 0.f;
			m_fTracePacketTime = 0.f;
			m_fPatrolPacketTime = 0.f;
			m_Owner->m_bMove = false;
		}
			break;
		case AI_Type::Skill1:
		{
			
		}
			break;
		case AI_Type::Skill2:
		{
			
		}
			break;
		case AI_Type::Patrol:
		{
			m_bAttackEnable = true;
			m_fAttackTime = 0.f;
			m_fTracePacketTime = 0.f;
			//ComputePatrol(fTime);

			switch (m_AIProtocol)
			{                                                                                                      
			case MA_PATROLSTART:
			{
				PatrolStart();
			}
				break;
			case MA_PATROL:
			{
				Patrol(fTime);
			}
				break;
			case MA_PATROLEND:
			{
				PatrolEnd();
			}
				break;
			}
		}
			break;
	}

	// 이전 인공지능 타입을 변경
	m_PrevAIType = m_AIType;
}

CAIDefault* CAIDefault::Clone() const
{
	return new CAIDefault(*this);
}
