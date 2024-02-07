
#include "AIAgent.h"
#include "Stream.h"
#include "../GameObject/NPC/Monster.h"
#include "../Level/Level.h"
#include "../GameObject/NPC/MonsterSpawnPoint.h"
#include "../UserManager.h"
#include "../User.h"
#include "../GameManager.h"
#include "../Level/Level.h"
#include "../Level/GameArea.h"

CAIAgent::CAIAgent() :
	m_AIType(AI_Type::Idle),
	m_PrevAIType(AI_Type::Idle),
	m_Owner(nullptr),
	m_fDetectTime(0.f),
	m_fDetectTimeMax(0.2f), // 0.1f
	m_fTargetDistance(0.f),
	m_fTracePacketTime(0.f),
	m_fTracePacketTimeMax(0.1f),
	m_fPatrolPacketTime(0.f),
	m_fPatrolPacketTimeMax(0.2f),
	m_fAttackTime(0.f),
	m_bAttackEnable(false),
	m_bDamageEnable(false),
	m_TargetTime(0.f),
	m_TargetTimeMax(0.1f),
	m_fPatrolTimeMax(0.2f),
	m_fPatrolTime(0.f),
	m_fPatrolWaitTimeMax(10.f),
	m_fPatrolWaitTime(0.f),
	m_fIdleTimeMax(1.0f),
	m_fIdleTime(0.f),
	m_AIProtocol(GameObjectProtocol::MA_IDLE),
	m_bDetectTargetEnable(true),
	m_DetectType(0),
	m_PatrolEnable(true),
	m_iPatrolIndex(1),
	m_PatrolStart(true),
	m_PatrolWaitEnable(true)
{

}

CAIAgent::CAIAgent(const CAIAgent& Agent)
{
	*this = Agent;

	m_PatrolStart = true;
	m_PatrolWaitEnable = true;
}

CAIAgent::~CAIAgent()
{
}

bool CAIAgent::Init()
{
	m_pTarget = nullptr;

	return true;
}

void CAIAgent::Update(float fTime)
{
	m_fDetectTime += fTime;

	// ������� ��� �ֺ��� �������� ������ ���ٴ� ���̹Ƿ� �ֺ��� Ž���Ͽ� ������ �ִ��� ã�´�.
	if (m_fDetectTime >= m_fDetectTimeMax)
	{
		m_fDetectTime -= m_fDetectTimeMax;

		CGameObject* TargetUser = nullptr;

		CGameArea* Area = m_Owner->GetLevel()->GetArea(m_Owner->GetPos());
		const list<class CUser*>* pCurrentUserList = Area->GetUserList();// ���� ���� Area�� User�� �������?
		list<class CUser*> UserList;
		// �ֺ� 8������ ������ �˻��Ͽ� �ش� ������ ���� ���Ϳ� ���� ���� �Ÿ��� ���Ͽ�
		// ������ �������� �ȿ� ���� ��� �ش� ������ �����ִ� �������� ����Ʈ�� ���Խ�Ų��.

		{
			auto	iter = pCurrentUserList->begin();
			auto	iterEnd = pCurrentUserList->end();
			for (; iter != iterEnd; ++iter)
			{
				UserList.push_back(*iter);
			}
		}

		int	CurrentIndexH, CurrentIndexV, CurrentIndex;

		CurrentIndexH = m_Owner->GetLevel()->GetAreaIndexH(m_Owner->GetPos());
		CurrentIndexV = m_Owner->GetLevel()->GetAreaIndexV(m_Owner->GetPos());
		CurrentIndex = m_Owner->GetLevel()->GetAreaIndex(m_Owner->GetPos());

		int	AreaHCount = m_Owner->GetLevel()->GetAreaCountH();
		int AreaVCount = m_Owner->GetLevel()->GetAreaCountV();

		const Vector3& AreaMin = Area->GetMin();
		const Vector3& AreaMax = Area->GetMax();

		for (int i = -1; i <= 1; ++i)
		{
			int	IndexV = CurrentIndexV + i;

			if (IndexV < 0 || IndexV >= AreaVCount)
				continue;

			for (int j = -1; j <= 1; ++j)
			{
				if (i == 0 && j == 0)
					continue;

				int	IndexH = CurrentIndexH + j;

				if (IndexH < 0 || IndexH >= AreaHCount)
					continue;

				int	Index = IndexV * AreaHCount + IndexH;

				bool	DistanceComplete = false;
				CGameArea* CurrentArea = nullptr;

				// ���� �Ʒ� �밢���� ���
				if (i == -1 && j == -1)
				{
					// ���� ������ Min�� ������ �Ÿ��� ���Ѵ�.
					float Distance = AreaMin.Distance(m_Owner->GetPos());

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				// �Ʒ�
				else if (i == -1 && j == 0)
				{
					float Distance = m_Owner->GetPos().x - AreaMin.x;

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				// ������ �Ʒ�
				else if (i == -1 && j == 1)
				{
					float Distance = AreaMin.Distance(m_Owner->GetPos());

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				else if (i == 0 && j == -1)
				{
					float Distance = m_Owner->GetPos().y - AreaMin.y;

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				else if (i == 0 && j == 1)
				{
					float Distance = AreaMin.y - m_Owner->GetPos().y;

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				else if (i == 1 && j == -1)
				{
					float Distance = AreaMin.Distance(m_Owner->GetPos());

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				else if (i == 1 && j == 0)
				{
					float Distance = AreaMin.x - m_Owner->GetPos().x;

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				else if (i == 1 && j == 1)
				{
					float Distance = AreaMin.Distance(m_Owner->GetPos());

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				if (DistanceComplete)
				{
					// 8���� Area�� ���� �������� ���� �����´�.
					pCurrentUserList = CurrentArea->GetUserList();

					auto	iter = pCurrentUserList->begin();
					auto	iterEnd = pCurrentUserList->end();

					for (; iter != iterEnd; ++iter)
					{
						UserList.push_back(*iter);
					}
				}

			}
		}



		// ����Ʈ�� ����ִٸ� ó�� ���� ������� �߰��Ǵ� ����� �����ٴ°��� boo������
		// ǥ���Ѵ�.
		bool	First = false;
		if (m_AggroList.empty())
		{
			First = true;
		}

		float TargetUserDist = 1000000.f;

		auto	iter = UserList.begin();
		auto	iterEnd = UserList.end();

		for (; iter != iterEnd; ++iter)
		{
			float Dist = m_Owner->GetPos().Distance((*iter)->GetPos());

			if (Dist < m_Owner->GetTraceRange())
			{
				bool	IncludedUser = false;

				auto	iter1 = m_AggroList.begin();
				auto	iter1End = m_AggroList.end();

				for (; iter1 != iter1End; ++iter1)
				{
					if ((*iter1).Dest == *iter)
					{
						IncludedUser = true;
						break;
					}
				}

				if (!IncludedUser)
				{
					if (TargetUserDist > Dist) // ���� ����� ��ü�� Ÿ������ �������
					{
						TargetUser = *iter;
						TargetUserDist = Dist;
					}

					// ���� ������ �ʴ��� �ϴ� ��׷ο� �־��ش�.
					AggravationInfo	AggroInfo;
					AggroInfo.Dest = *iter;
					AggroInfo.Point = 0;

					m_AggroList.push_back(AggroInfo);
				}
			}
		}

		// AggroList�� ������ ���尡��� Ÿ�ٿ��Ը� ��׷�����Ʈ 10���� �ο��Ѵ�.
		if (First)
		{
			auto	iter1 = m_AggroList.begin();
			auto	iter1End = m_AggroList.end();

			for (; iter1 != iter1End; ++iter1)
			{
				if ((*iter1).Dest == TargetUser)
				{
					(*iter1).Point = 10;
					break;
				}
			}
		}

		else
		{
			TargetUser = nullptr;

			auto	iter1 = m_AggroList.begin();
			auto	iter1End = m_AggroList.end();

			int	GreatPoint = -1;

			for (; iter1 != iter1End;)
			{
				if ((*iter1).Dest->IsDeath())
				{
					iter1 = m_AggroList.erase(iter1);
					iter1End = m_AggroList.end();
					continue;
				}

				else if ((*iter1).Dest->GetObjType() == GameObject_Type::User)
				{
					if (!((CUser*)(*iter1).Dest)->GetConnect())
					{
						iter1 = m_AggroList.erase(iter1);
						iter1End = m_AggroList.end();
						continue;
					}
				}

				if ((*iter1).Dest != m_pTarget)
				{
					(*iter1).Time += fTime;

					if ((*iter1).Time >= (*iter1).TimeMax)
					{
						(*iter1).Time -= (*iter1).TimeMax;
						(*iter1).Point -= 5;

						if ((*iter1).Point < 0)
							(*iter1).Point = 0;
					}
				}

				else
				{
					(*iter1).Time = 0.f;
				}

				if ((*iter1).Point > GreatPoint)
				{
					GreatPoint = (*iter1).Point;
					TargetUser = (*iter1).Dest;
				}

				++iter1;
			}
		}

		m_pTarget = TargetUser;

	}
}

CAIAgent* CAIAgent::Clone() const
{
	return nullptr;
}


void CAIAgent::CheckTarget(float fTime)
{
	// CAIAgent���� ������ AI�� Trace��� Target�� ������ ������ ���̴�.
	// Target�� ���ٸ� AIDefault���� AI�� Idle�� ������ �ȴ�.
	// �����ð��� �ѹ��� �ܼ� �Ѿư��� �������� ���������� �Ǵ��ؾ� �Ѵ�.
	// ���� 1�ʿ� 10���� �ϱ��ϴµ� �ϴ� �������� �������� üũ�� �ϴ� �Լ�

	m_TargetTime += fTime;
	if (m_TargetTime >= m_TargetTimeMax)
	{
		m_TargetTime -= m_TargetTimeMax;

		Vector3	vTargetPos = m_pTarget->GetPos();
		Vector3	vPos = m_Owner->GetPos();
		Vector3	vDir = vTargetPos - vPos;

		//vPos.z = vTargetPos.z;
		//���� ������ҽ� ���������� ���Ͱ� ���ݽ� ������ ���� ��ġ�� Ʋ�������ִ�.

		float	fDist = vPos.Distance(vTargetPos);
		//vDir.z = 0.f;//����
		vDir.Normalize();

		if (fDist <= m_Owner->GetAttackRange())
		{
			m_AIType = AI_Type::Attack;
			m_AIProtocol = GameObjectProtocol::MA_ATTACK;
		}

		else if (fDist <= m_Owner->GetTraceRange() && m_bAttackEnable)
		{
			// Ÿ����ġ�� ������ �Ǿ��ٸ�
			// Ÿ���� TraceRange���� ������ ����ؼ� �����̴ϱ� ����ؼ� �����°� ����.
			if (m_vTargetPos.x != vTargetPos.x || m_vTargetPos.y != vTargetPos.y)
			{
				//cout << "Ÿ���� �߰ݹ��������� �����̰� �ֽ��ϴ�......" << '\n';
				m_AIType = AI_Type::Trace;
				//m_AIProtocol = GameObjectProtocol::MA_TRACESTART;
				m_AIProtocol = GameObjectProtocol::MA_TRACE;
				m_fAttackTime = 0.f;

				m_vTargetPos = vTargetPos;

				CStream	 stream;
				BYTE 	Packet[MAX_BUFFER_LENGTH] = {};
				stream.SetBuffer((char*)Packet);

				ELevel	LevelType = m_Owner->GetLevelType();
				Vector3 vPos = m_Owner->GetPos();

				// ���� ã���� �ϴ� ������̼��� �̸��� �Ѱ��ش�.
				int	iNavNameLength = (int)m_Owner->m_NavName.length();
				stream.Write(&iNavNameLength, 4);
				stream.Write(m_Owner->m_NavName.c_str(), iNavNameLength);

				int	iSerialNumber = m_Owner->GetSerialNumber();

				stream.Write(&iSerialNumber, 4);
				stream.Write(&LevelType, sizeof(ELevel));
				stream.Write(&vPos, 12);
				stream.Write(&m_vTargetPos, 12);

				if (CGameManager::GetInst()->GetAISession())
					CGameManager::GetInst()->GetAISession()->WritePacket(GP_FINDMONSTERPATH, Packet, stream.GetLength());
			}
		}
		else if (fDist > m_Owner->GetTraceRange())
		{
			m_AIType = AI_Type::Idle;
			m_AIProtocol = GameObjectProtocol::MA_IDLE;
			m_PathList.clear();

			DeleteAggroDest(m_pTarget);

			m_vTargetPos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
			m_pTarget = nullptr;
			m_bAttackEnable = true;
			m_fAttackTime = 0.f;
			m_TargetTime = 0.f;
		}
		else
		{
			m_AIType = AI_Type::Idle;
			m_AIProtocol = GameObjectProtocol::MA_IDLE;
			m_PathList.clear();

			m_vTargetPos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
			m_pTarget = nullptr;
			m_bAttackEnable = true;
			m_fAttackTime = 0.f;
			m_TargetTime = 0.f;
		}
	}
}

void CAIAgent::TargetTrace(float fTime)
{
	if (!m_pTarget)
	{
		m_PathList.clear();
		m_AIType = AI_Type::Idle;
		return;
	}
	else if (m_PathList.empty())
	{
		return;
	}

	// �ܿ��⼭ ���س��� ���� ã�Ƽ� �̵��ϵ��� ó���Ѵ�.
	// �ش� ��ġ�� �����ϱ����� ���� ��ġ�� �˷���� �ϴµ� .
	// �̵��� ��ġ�� ���´�.
	Vector3	TargetPos = m_PathList.front();
	Vector3	Pos = m_Owner->GetPos();

	// �ϴ� �÷��̾��� Pos�� �츮�� ������ ��ǥ�� ĳ���� ����̱� ������
	// �׿� ���� ���ʹ� z�� 0�̱� ������ 
	// �÷��̾�� ������ �Ÿ����� z�� -���� �ɼ��� �ִ�.
	TargetPos.z = 0.f;
	Pos.z = 0.f;

	// �̵��� ��ġ���� ������ ���Ѵ�.
	Vector3	Dir = TargetPos - Pos;
	float	TargetDist = Dir.Length();

	Dir.Normalize();

	float	MoveDist = m_Owner->GetMoveSpeed() * fTime;

	// ���� ������ ����ȭ�� �Ͻ����� Ʈ���ȿ����� ���� ���������� �� �̵��������
	if (MoveDist > TargetDist)
	{
		MoveDist = TargetDist;

		m_PathList.pop_front();
	}

	Pos = m_Owner->GetPos();

	Pos += Dir * MoveDist;
	m_Owner->SetPos(Pos);

	m_Owner->SetMoveDir(Dir);

	m_Owner->SetMove(true);

	Pos.z = m_pTarget->GetPos().z;

	float	fDist = m_pTarget->GetPos().Distance(Pos); // �̵��� �Ÿ�����

	if (m_AIProtocol != MA_TRACESTART && m_AIProtocol != MA_TRACE && m_AIProtocol != MA_TRACEEND /*&& m_AIProtocol != MA_ATTACK*/)
	{
		m_AIProtocol = MA_TRACESTART;
	}

	else if (m_AIProtocol == MA_TRACESTART)
	{
		m_AIProtocol = MA_TRACE;
	}

	else if (fDist > m_Owner->GetTraceRange())
	{
		m_AIProtocol = MA_TRACEEND;
		m_pTarget = nullptr;
	}
}

void CAIAgent::ClearTarget()
{
	m_pTarget = nullptr;
}

void CAIAgent::ComputePatrol(float fTime)
{
	//if (m_pTarget)
	//{
	//	return;
	//}

	//// �ܿ��⼭ ���س��� ���� ã�Ƽ� �̵��ϵ��� ó���Ѵ�.
	//// �ش� ��ġ�� �����ϱ����� ���� ��ġ�� �˷���� �ϴµ� .
	//// �̵��� ��ġ�� ���´�.
	//Vector3	PatrolPos = m_vecPatrolPoint[m_iPatrolIndex];
	//Vector3	Pos = m_Owner->GetPos();

	//// �ϴ� �÷��̾��� Pos�� �츮�� ������ ��ǥ�� ĳ���� ����̱� ������
	//// �׿� ���� ���ʹ� z�� 0�̱� ������ 
	//// �÷��̾�� ������ �Ÿ����� z�� -���� �ɼ��� �ִ�.
	//PatrolPos.z = 0.f;
	//Pos.z = 0.f;

	//// �̵��� ��ġ���� ������ ���Ѵ�.
	//Vector3	Dir = PatrolPos - Pos;
	//float	TargetDist = Dir.Length();

	//if (TargetDist == 0.f)
	//{
	//	m_AIProtocol = MA_PATROLEND;
	//	m_iPatrolIndex = (m_iPatrolIndex + 1) % m_vecPatrolPoint.size();
	//}


	//{
	//	Dir.Normalize();

	//	float	MoveDist = m_Owner->GetMoveSpeed() * fTime;

	//	bool	Arrive = false;

	//	// ���� ������ ����ȭ�� �Ͻ����� Ʈ���ȿ����� ���� ���������� �� �̵��������
	//	if (MoveDist >= TargetDist)
	//	{
	//		MoveDist = TargetDist;

	//		Arrive = true;
	//	}

	//	Pos = m_Owner->GetPos();

	//	Pos += Dir * MoveDist;
	//	m_Owner->SetPos(Pos);
	//	m_Owner->SetMoveDir(Dir);
	//	m_Owner->SetMove(true);

	//	Pos.z = m_vecPatrolPoint[m_iPatrolIndex].z;

	//	if (Arrive)
	//	{
	//		m_AIProtocol = MA_PATROLEND;
	//		cout << ((CMonster*)m_Owner)->GetName() << ((CMonster*)m_Owner)->GetSpawnPoint()->GetSpawnMonsterNumber()
	//			<< "�� ����  " << m_iPatrolIndex << "�� ��ġ��  " << "TargetArrive..." << endl;

	//		m_iPatrolIndex = (m_iPatrolIndex + 1) % m_vecPatrolPoint.size();
	//	}

	//	else
	//	{
	//		if (m_AIProtocol != MA_PATROLSTART && m_AIProtocol != MA_PATROL && m_AIProtocol != MA_PATROLEND)
	//		{
	//			m_AIProtocol = MA_PATROLSTART;
	//		}

	//		else if (m_AIProtocol == MA_PATROLSTART)
	//		{
	//			m_AIProtocol = MA_PATROL;

	//			cout << ((CMonster*)m_Owner)->GetName() << ((CMonster*)m_Owner)->GetSpawnPoint()->GetSpawnMonsterNumber()
	//				<< "�� ����" << "Patrol...." << endl;
	//		}
	//	}
	//}
}

void CAIAgent::Idle()
{
	m_vTargetPos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_pTarget = nullptr;
	m_TargetTime = 0.f;
}

void CAIAgent::Attack()
{
	// ����� �׳� ���ݸ���� ������ �뵵
	m_PathList.clear();

	if (m_bAttackEnable)
	{
		//cout << "ServerPlayer ��ġ : " << m_vTargetPos.x << "  /  " << m_vTargetPos.y << endl;
		cout << "Attack Situlation - ServerPlayer	��ġ : " << m_pTarget->GetPos().x << "  /  " << m_pTarget->GetPos().y << endl;
		cout << "Attack Situlation - ServerMonster	��ġ : " << m_Owner->GetPos().x << "  /  " << m_Owner->GetPos().y << endl;

		cout << "Attack Situlation - ServerPlayer	���� : " << m_pTarget->GetMoveDir().x << "  /  " << m_pTarget->GetMoveDir().y << endl;
		cout << "Attack Situlation - ServerMonster	���� : " << m_Owner->GetMoveDir().x << "  /  " << m_Owner->GetMoveDir().y << endl;

		int	m_iSerialNumber = m_Owner->GetSerialNumber();
		m_AIProtocol = MA_ATTACK;
		m_bAttackEnable = false;
		m_bDamageEnable = true;
		m_Owner->SetMove(false);
		m_fTracePacketTime = 0.f;

		CStream stream;
		char packet[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(packet);
		stream.Write(&m_iSerialNumber, sizeof(int));
		stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));

		const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
		auto	iter = m_pUserList->begin();
		auto	iterEnd = m_pUserList->end();
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
		}
	}
}

void CAIAgent::Hit(float fTime)
{
	// ����� ���� ����������� �ϱ����� ������üũ
	m_fAttackTime += fTime;

	//m_fAttackTime = 0.f;
	//m_fAttackEnableTime = 0.286f;
	//m_fAttackEnd = 1.17f;

	Vector3	vPos = m_Owner->GetPos();
	Vector3	vDir = m_Owner->GetMoveDir();

	if (m_bDamageEnable)
	{
		if (m_fAttackTime >= m_fAttackEnableTime)
		{
			m_bDamageEnable = false;
			m_AIProtocol = MA_DAMAGE;
			int	m_iSerialNumber = m_Owner->GetSerialNumber();
			// Ŭ���̾�Ʈ ���Ϳ��� �����϶�� ��Ŷ�� �����ֵ��� �Ѵ�.

			CStream stream;
			char packet[MAX_BUFFER_LENGTH] = {};
			stream.SetBuffer(packet);
			stream.Write(&m_iSerialNumber, sizeof(int));
			stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));

			CUser* pPlayer = (CUser*)m_pTarget;

			int	iDamage = (m_Owner->m_iAttack) - pPlayer->GetArmor();

			cout << m_iSerialNumber << "�� Monster�� " << iDamage << "�������� �������ϴ�....." << endl;

			iDamage = iDamage < 1 ? 1 : iDamage;

			iDamage = 10; // ������

			stream.Write(&iDamage, sizeof(int));

			pPlayer->AddHP(iDamage);

			int	iHP = pPlayer->GetHP();
			stream.Write(&iHP, sizeof(int));

			int	iChacterNumber = pPlayer->GetCharacterNumber();
			stream.Write(&iChacterNumber, sizeof(int));

			const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
			auto	iter = m_pUserList->begin();
			auto	iterEnd = m_pUserList->end();
			for (; iter != iterEnd; ++iter)
			{
				(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
			}
		}
	}
	else
	{
		if (m_fAttackTime >= m_fAttackEnd)
		{
			m_bAttackEnable = true;
			m_fAttackTime -= m_fAttackEnd;

			Vector3	vTargetPos = m_pTarget->GetPos();
			Vector3	vOwnerPos = m_Owner->GetPos();
			vOwnerPos.z = 0.f;
			vTargetPos.z = 0.f;

			Vector3	vDir = vTargetPos - vOwnerPos;
			vDir.Normalize();

			m_AIProtocol = MA_ATTACKEND;
			int	m_iSerialNumber = m_Owner->GetSerialNumber();
			// Ŭ���̾�Ʈ ���Ϳ��� �����϶�� ��Ŷ�� �����ֵ��� �Ѵ�.

			CStream stream;
			char packet[MAX_BUFFER_LENGTH] = {};
			stream.SetBuffer(packet);
			stream.Write(&m_iSerialNumber, sizeof(int));
			stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));

			stream.Write(&vDir, sizeof(Vector3));

			const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
			auto	iter = m_pUserList->begin();
			auto	iterEnd = m_pUserList->end();
			for (; iter != iterEnd; ++iter)
			{
				(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
			}
		}
	}
}

void CAIAgent::Skill1(float fTime)
{


}

void CAIAgent::SKill2(float fTime)
{
}

void CAIAgent::Patrol(float fTime)
{
	/*m_fPatrolPacketTime += fTime;

	if (m_fPatrolPacketTime >= m_fPatrolPacketTimeMax)
	{
		m_fPatrolPacketTime -= m_fPatrolPacketTimeMax;

		CStream stream;
		char packet[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(packet);

		Vector3	vPos = m_Owner->GetPos();
		Vector3	vDir = m_Owner->GetMoveDir();
		int	m_iSerialNumber = m_Owner->GetSerialNumber();

		stream.Write(&m_iSerialNumber, sizeof(int));
		stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));
		stream.Write(&vDir, sizeof(Vector3));
		stream.Write(&vPos, sizeof(Vector3));

		const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
		auto	iter = m_pUserList->begin();
		auto	iterEnd = m_pUserList->end();
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
		}
	}*/
}

void CAIAgent::PatrolStart()
{
	CStream stream;
	char packet[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(packet);

	int	m_iSerialNumber = m_Owner->GetSerialNumber();

	stream.Write(&m_iSerialNumber, sizeof(int));
	stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));

	Vector3	PatrolPos = m_vecPatrolPoint[m_iPatrolIndex];
	Vector3	Pos = m_Owner->GetPos();

	// �ϴ� �÷��̾��� Pos�� �츮�� ������ ��ǥ�� ĳ���� ����̱� ������
	// �׿� ���� ���ʹ� z�� 0�̱� ������ 
	// �÷��̾�� ������ �Ÿ����� z�� -���� �ɼ��� �ִ�.
	PatrolPos.z = 0.f;
	Pos.z = 0.f;

	// �̵��� ��ġ���� ������ ���Ѵ�.
	Vector3	Dir = PatrolPos - Pos;
	Dir.Normalize();

	stream.Write(&Dir, sizeof(Vector3));


	cout << m_iSerialNumber << "�� ����  ----  Patrol Start ....." << endl;

	const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
	auto	iter = m_pUserList->begin();
	auto	iterEnd = m_pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	}
}

void CAIAgent::PatrolEnd()
{
	m_fPatrolPacketTime = 0.f;

	CStream stream;
	char packet[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(packet);

	Vector3	vPos = m_Owner->GetPos();
	Vector3	vDir = m_Owner->GetMoveDir();
	int	m_iSerialNumber = m_Owner->GetSerialNumber();

	stream.Write(&m_iSerialNumber, sizeof(int));
	stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));
	stream.Write(&vPos, sizeof(Vector3));

	const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
	auto	iter = m_pUserList->begin();
	auto	iterEnd = m_pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	}
}

void CAIAgent::Trace(float fTime)
{
	m_fTracePacketTime += fTime;

	if (m_fTracePacketTime >= m_fTracePacketTimeMax)
	{
		m_fTracePacketTime -= m_fTracePacketTimeMax;

		CStream stream;
		char packet[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(packet);

		Vector3	vPos = m_Owner->GetPos();
		Vector3	vDir = m_Owner->GetMoveDir();
		int	m_iSerialNumber = m_Owner->GetSerialNumber();

		stream.Write(&m_iSerialNumber, sizeof(int));
		stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));
		stream.Write(&vDir, sizeof(Vector3));
		stream.Write(&vPos, sizeof(Vector3));

		const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
		auto	iter = m_pUserList->begin();
		auto	iterEnd = m_pUserList->end();
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
		}
	}
}

void CAIAgent::TraceStart()
{
	CStream stream;
	char packet[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(packet);

	int	m_iSerialNumber = m_Owner->GetSerialNumber();

	stream.Write(&m_iSerialNumber, sizeof(int));
	stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));

	const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
	auto	iter = m_pUserList->begin();
	auto	iterEnd = m_pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	}
}

void CAIAgent::TraceEnd()
{
	m_fTracePacketTime = 0.f;

	CStream stream;
	char packet[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(packet);

	Vector3	vPos = m_Owner->GetPos();
	Vector3	vDir = m_Owner->GetMoveDir();
	int	m_iSerialNumber = m_Owner->GetSerialNumber();

	stream.Write(&m_iSerialNumber, sizeof(int));
	stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));
	stream.Write(&vPos, sizeof(Vector3));

	const list<class CUser*>* m_pUserList = m_Owner->GetLevel()->GetUserList();
	auto	iter = m_pUserList->begin();
	auto	iterEnd = m_pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	}
	m_AIProtocol = MA_IDLE;
}
