
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

	// 비어있을 경우 주변에 전투중인 유저가 없다는 것이므로 주변을 탐색하여 유저가 있는지 찾는다.
	if (m_fDetectTime >= m_fDetectTimeMax)
	{
		m_fDetectTime -= m_fDetectTimeMax;

		CGameObject* TargetUser = nullptr;

		CGameArea* Area = m_Owner->GetLevel()->GetArea(m_Owner->GetPos());
		const list<class CUser*>* pCurrentUserList = Area->GetUserList();// 현재 속한 Area에 User가 몇명인지?
		list<class CUser*> UserList;
		// 주변 8방향의 영역을 검사하여 해당 영역이 현재 몬스터와 영역 끝의 거리를 비교하여
		// 몬스터의 인지범위 안에 있을 경우 해당 영역에 속해있는 유저들을 리스트에 포함시킨다.

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

				// 왼쪽 아래 대각선일 경우
				if (i == -1 && j == -1)
				{
					// 현재 영역의 Min과 몬스터의 거리를 구한다.
					float Distance = AreaMin.Distance(m_Owner->GetPos());

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				// 아래
				else if (i == -1 && j == 0)
				{
					float Distance = m_Owner->GetPos().x - AreaMin.x;

					if (m_Owner->GetTraceRange())
					{
						DistanceComplete = true;
						CurrentArea = m_Owner->GetLevel()->GetArea(IndexH, IndexV);
					}
				}

				// 오른쪽 아래
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
					// 8방향 Area에 속한 유저들을 전부 가져온다.
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



		// 리스트가 비어있다면 처음 공격 대상으로 추가되는 대상을 만들어낸다는것을 boo변수로
		// 표현한다.
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
					if (TargetUserDist > Dist) // 가장 가까운 물체를 타겟으로 잡기위해
					{
						TargetUser = *iter;
						TargetUserDist = Dist;
					}

					// 가장 가깝지 않더라도 일단 어그로에 넣어준다.
					AggravationInfo	AggroInfo;
					AggroInfo.Dest = *iter;
					AggroInfo.Point = 0;

					m_AggroList.push_back(AggroInfo);
				}
			}
		}

		// AggroList가 없을떄 가장가까운 타겟에게만 어그로포인트 10점을 부여한다.
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
	// CAIAgent에서 결정된 AI가 Trace라면 Target은 무조건 존재할 것이다.
	// Target이 없다면 AIDefault에서 AI가 Idle로 결정이 된다.
	// 일정시간에 한번씩 단순 쫓아가는 상태인지 공격인지를 판단해야 한다.
	// 현재 1초에 10번씩 하긴하는데 일단 공격인지 추적인지 체크만 하는 함수

	m_TargetTime += fTime;
	if (m_TargetTime >= m_TargetTimeMax)
	{
		m_TargetTime -= m_TargetTimeMax;

		Vector3	vTargetPos = m_pTarget->GetPos();
		Vector3	vPos = m_Owner->GetPos();
		Vector3	vDir = vTargetPos - vPos;

		//vPos.z = vTargetPos.z;
		//수정 적용안할시 여러마리의 몬스터가 공격시 임의의 몬스터 위치가 틀어질수있다.

		float	fDist = vPos.Distance(vTargetPos);
		//vDir.z = 0.f;//수정
		vDir.Normalize();

		if (fDist <= m_Owner->GetAttackRange())
		{
			m_AIType = AI_Type::Attack;
			m_AIProtocol = GameObjectProtocol::MA_ATTACK;
		}

		else if (fDist <= m_Owner->GetTraceRange() && m_bAttackEnable)
		{
			// 타겟위치가 변경이 되었다면
			// 타겟이 TraceRange범위 내에서 계속해서 움직이니까 계속해서 들어오는거 같다.
			if (m_vTargetPos.x != vTargetPos.x || m_vTargetPos.y != vTargetPos.y)
			{
				//cout << "타겟이 추격범위내에서 움직이고 있습니다......" << '\n';
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

				// 현재 찾고자 하는 내비게이션의 이름을 넘겨준다.
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

	// ●여기서 구해놓은 길을 찾아서 이동하도록 처리한다.
	// 해당 위치로 도착하기전에 다음 위치를 알려줘야 하는데 .
	// 이동할 위치를 얻어온다.
	Vector3	TargetPos = m_PathList.front();
	Vector3	Pos = m_Owner->GetPos();

	// 일단 플레이어의 Pos는 우리가 설정한 좌표는 캐릭터 가운데이기 떄문에
	// 그에 반해 몬스터는 z가 0이기 떄문에 
	// 플레이어와 몬스터의 거리에서 z가 -값이 될수가 있다.
	TargetPos.z = 0.f;
	Pos.z = 0.f;

	// 이동할 위치로의 방향을 구한다.
	Vector3	Dir = TargetPos - Pos;
	float	TargetDist = Dir.Length();

	Dir.Normalize();

	float	MoveDist = m_Owner->GetMoveSpeed() * fTime;

	// 만약 서버의 과부화나 일시적인 트래픽오류로 인해 목적지보다 더 이동했을경우
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

	float	fDist = m_pTarget->GetPos().Distance(Pos); // 이동후 거리차이

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

	//// ●여기서 구해놓은 길을 찾아서 이동하도록 처리한다.
	//// 해당 위치로 도착하기전에 다음 위치를 알려줘야 하는데 .
	//// 이동할 위치를 얻어온다.
	//Vector3	PatrolPos = m_vecPatrolPoint[m_iPatrolIndex];
	//Vector3	Pos = m_Owner->GetPos();

	//// 일단 플레이어의 Pos는 우리가 설정한 좌표는 캐릭터 가운데이기 떄문에
	//// 그에 반해 몬스터는 z가 0이기 떄문에 
	//// 플레이어와 몬스터의 거리에서 z가 -값이 될수가 있다.
	//PatrolPos.z = 0.f;
	//Pos.z = 0.f;

	//// 이동할 위치로의 방향을 구한다.
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

	//	// 만약 서버의 과부화나 일시적인 트래픽오류로 인해 목적지보다 더 이동했을경우
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
	//			<< "번 몬스터  " << m_iPatrolIndex << "번 위치에  " << "TargetArrive..." << endl;

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
	//				<< "번 몬스터" << "Patrol...." << endl;
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
	// 여기는 그냥 공격모션을 보내는 용도
	m_PathList.clear();

	if (m_bAttackEnable)
	{
		//cout << "ServerPlayer 위치 : " << m_vTargetPos.x << "  /  " << m_vTargetPos.y << endl;
		cout << "Attack Situlation - ServerPlayer	위치 : " << m_pTarget->GetPos().x << "  /  " << m_pTarget->GetPos().y << endl;
		cout << "Attack Situlation - ServerMonster	위치 : " << m_Owner->GetPos().x << "  /  " << m_Owner->GetPos().y << endl;

		cout << "Attack Situlation - ServerPlayer	방향 : " << m_pTarget->GetMoveDir().x << "  /  " << m_pTarget->GetMoveDir().y << endl;
		cout << "Attack Situlation - ServerMonster	방향 : " << m_Owner->GetMoveDir().x << "  /  " << m_Owner->GetMoveDir().y << endl;

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
	// 여기는 실제 데미지계산을 하기위한 프레임체크
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
			// 클라이언트 몬스터에게 공격하라는 패킷을 보내주도록 한다.

			CStream stream;
			char packet[MAX_BUFFER_LENGTH] = {};
			stream.SetBuffer(packet);
			stream.Write(&m_iSerialNumber, sizeof(int));
			stream.Write(&m_AIProtocol, sizeof(GameObjectProtocol));

			CUser* pPlayer = (CUser*)m_pTarget;

			int	iDamage = (m_Owner->m_iAttack) - pPlayer->GetArmor();

			cout << m_iSerialNumber << "번 Monster가 " << iDamage << "데미지를 입혔습니다....." << endl;

			iDamage = iDamage < 1 ? 1 : iDamage;

			iDamage = 10; // 없애자

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
			// 클라이언트 몬스터에게 공격하라는 패킷을 보내주도록 한다.

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

	// 일단 플레이어의 Pos는 우리가 설정한 좌표는 캐릭터 가운데이기 떄문에
	// 그에 반해 몬스터는 z가 0이기 떄문에 
	// 플레이어와 몬스터의 거리에서 z가 -값이 될수가 있다.
	PatrolPos.z = 0.f;
	Pos.z = 0.f;

	// 이동할 위치로의 방향을 구한다.
	Vector3	Dir = PatrolPos - Pos;
	Dir.Normalize();

	stream.Write(&Dir, sizeof(Vector3));


	cout << m_iSerialNumber << "번 몬스터  ----  Patrol Start ....." << endl;

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
