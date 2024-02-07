
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
	// �θ��� Update���� Target�� ����ش�.
	CAIAgent::Update(fTime);

	int	SerialNumber = m_Owner->GetSerialNumber();
	auto pUserList = m_Owner->GetLevel()->GetUserList();
	bool	FindPath = false;
	/*
	��Ʈ�� �߿� ������ �����ϸ� �ش� ������ PatrolStart�� ���޴´�.
	�׷��Ƿ� ������ ���������� ���� ���� ������ �Ѱ��ٶ� ��Ʈ�� ������ �������� ���� �Ѱ��־�� �Ѵ�.
	�̴� ���Ͱ� �ٸ� ������ �Ѱ� ������ Trace ���µ� ���������� �Ѱ��־�� �Ѵ�.
	*/

	if (m_pTarget)
	{
		// Trace��� FindPath���� ������.
		CheckTarget(fTime);
		// Trace���� ������� ��׷�List�� UserList���� ���ܽ��Ѿ��Ѵ�.

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
				// Patrol�� �����ؾ� �ϴ� ��Ȳ�̶�� �̵��ؾ��� ��Ʈ�� ����Ʈ�� ��ġ�� ���ͼ�
				// �� ��ġ�� ���ϴ� ������ ���ϰ� �̵��� �����ش�.
				// ��, ��Ʈ�ѷ� ó�� �ٲ� ������ �̵� ����� ��ġ�� �����ְ� �� ���� �����Ӻ���
				// �̵��� �ǽ��Ѵ�.
				Vector3	PatrolPos = m_vecPatrolPoint[m_iPatrolIndex];
				Vector3	Pos = m_Owner->GetPos();

				// �ϴ� �÷��̾��� Pos�� �츮�� ������ ��ǥ�� ĳ���� ����̱� ������
				// �׿� ���� ���ʹ� z�� 0�̱� ������
				// �÷��̾�� ������ �Ÿ����� z�� -���� �ɼ��� �ִ�.
				PatrolPos.z = 0.f;
				Pos.z = 0.f;

				Vector3	Dir = PatrolPos - Pos;
				float	TargetDist = Dir.Length();

				// �̹� ������ ������ ����� �Ͼ ���� �����Ƿ� �Ÿ��� 0�� ��� �������� ó���Ѵ�.
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
							"�� ����  " << m_iPatrolIndex << "�� ��ġ��  " << "Patrol Arrive..." << endl;
						m_iPatrolIndex = (m_iPatrolIndex + 1) % m_vecPatrolPoint.size();
					}

					else
					{
						m_AIProtocol = MA_PATROL;
					}
				}
			}
		}

		else // Patrol�� ������
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

	// ���� �ΰ����� Ÿ���� ����
	m_PrevAIType = m_AIType;
}

CAIDefault* CAIDefault::Clone() const
{
	return new CAIDefault(*this);
}
