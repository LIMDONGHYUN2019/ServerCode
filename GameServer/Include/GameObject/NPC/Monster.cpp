#include "Monster.h"
#include "MonsterSpawnPoint.h"
#include "../../UserManager.h"
#include "../../User.h"
#include "Stream.h"
#include "../../Level/Level.h"
#include "../../GameManager.h"
#include <random>

CMonster::CMonster() :
	m_MonsterAIType(AI_Type::Idle),
	m_pSpawnPoint(nullptr),
	m_AIProtocol(MA_IDLE)
{
	m_eObjType = GameObject_Type::Monster;
}

CMonster::CMonster(const CMonster& monster)	:
	CNPC(monster)
{
	*this = monster;

	m_pLevel = nullptr;
}

CMonster::~CMonster()
{
	if (m_pSpawnPoint)
		m_pSpawnPoint->MonsterDeath();
}

bool CMonster::Init()
{
	if (!CNPC::Init())
		return false;

	return true;
}
   
bool CMonster::Init(FILE* pFile)
{
	if (!CNPC::Init(pFile))
		return false;

	fread(&m_iAttack, 4, 1, pFile);
	fread(&m_iArmor, 4, 1, pFile);

	fread(&m_iHPMax, 4, 1, pFile);
	m_iHP = m_iHPMax;

	fread(&m_iLevel, 4, 1, pFile);
	fread(&m_iExp, 4, 1, pFile);
	fread(&m_iGold, 4, 1, pFile);

	fread(&m_fMoveSpeed, 4, 1, pFile);
	fread(&m_fTraceRange, 4, 1, pFile);
	fread(&m_fAttackRange, 4, 1, pFile);

	return true;
}

void CMonster::Update(float fTime)
{
	CNPC::Update(fTime);

	//m_fDetectTime += fTime;
	////const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	//const list<class CUser*>* pUserList = m_Level->GetUserList();

	//// 여기서는 모든 몬스터들이 공통으로 사용을 할 수 있게 공격대상을 찾아주는 작업을 진행하고 실제 패킷은 하위 클래스에서 보내줄 수
	//// 있도록 한다.

	///*
	//	타겟이 움직일 경우라면 당연히 경로를 다시 계산을 해주어야 할것이다.
	//	하지만 만약 타겟이 같은 셀 안에서만 움직였을 경우라면 즉, 다른 셀로 이동을 한 것이 아니라면 경로를 다시 찾을 필요는 없을 것이다.
	//	타겟의 위치가 제자리에 있거나 혹은 다른 셀로 이동을 했는지를 판단해주어야 한다.
	//*/
	//if (m_fDetectTime >= m_fDetectTimeMax)
	//{
	//	m_fDetectTime -= m_fDetectTimeMax;
	//	// 1초에 10번씩 탐색한다.
	//	if (m_pTarget)
	//	{
	//		// 현재 타겟이 죽었는지 판단한다.
	//		if (m_pTarget->IsDeath() || !m_pTarget->GetConnect())
	//		{
	//			m_TargetPos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//			m_pTarget = nullptr;
	//			m_TargetTime = 0.f;
	//			m_MonsterAIType = MonsterAI::Idle;
	//			m_AIProtocol = MonsterAIProtocol::MA_IDLE;

	//			CStream stream;
	//			char packet[MAX_BUFFER_LENGTH] = {};
	//			stream.SetBuffer(packet);
	//			stream.Write(&m_iSerialNumber, sizeof(int));
	//			stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));

	//			auto	iter = pUserList->begin();
	//			auto	iterEnd = pUserList->end();
	//			for (; iter != iterEnd; ++iter)
	//			{
	//				(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//			}

	//			m_PathList.clear();
	//		}
	//	}
	//	else if(!m_bDeath)
	//	{
	//		// 가장 가까운 유저를 타겟으로 찾아준다.
	//		auto	iter = pUserList->begin();
	//		auto	iterEnd = pUserList->end();

	//		m_fTargetDistance = 10000.f;

	//		bool	Battle = false;

	//		for (; iter != iterEnd; ++iter)
	//		{
	//			if ((*iter)->IsDeath())
	//				continue;

	//			Vector3	vPos = (*iter)->GetPos();
	//			float	fDist = vPos.Distance(m_vPos);

	//			if (fDist <= m_fTraceRange) //1000
	//			{
	//				Battle = true;

	//				//if(fDist < m_fTargetDistance) // 아마도 실제 몬스터와의 거리를 변수로 두려는듯.
	//				//{
	//				//	m_pTarget = *iter;
	//				//	m_fTargetDistance = fDist;
	//				//}
	//				m_pTarget = *iter;
	//				m_fTargetDistance = fDist;

	//				m_TargetPos = vPos;
	//				// 쫒아가던중 타겟이 이동하지않으면 굳이 찾을필요가없도록 타켓의 이전위치를 받아온것.

	//				m_TargetTime = 0.f;
	//				break;
	//			}
	//		}

	//		if (!Battle)
	//		{
	//			m_TargetPos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//			m_pTarget = nullptr;
	//			m_TargetTime = 0.f;
	//			m_fTargetDistance = 100000.f;
	//			m_MonsterAIType = MonsterAI::Idle;
	//		}
	//	}
	//}

	//if (m_pTarget)
	//{
	//	m_TargetTime += fTime;

	//	if (m_TargetTime >= m_TargetTimeMax)
	//	{
	//		m_TargetTime -= m_TargetTimeMax;

	//		Vector3	vTargetPos = m_pTarget->GetPos();
	//		Vector3	vPos = m_vPos;
	//		Vector3	vDir = vTargetPos - vPos;

	//		vPos.z = vTargetPos.z;//수정 적용안할시 여러마리의 몬스터가 공격시 임의의 몬스터 위치가 틀어질수있다.

	//		float	fDist = vPos.Distance(vTargetPos);
	//		//vDir.z = 0.f;//수정
	//		vDir.Normalize();

	//		if (fDist <= m_fAttackRange)
	//		{
	//			m_PathList.clear();
	//			if (m_MonsterAIType != MonsterAI::Attack)
	//			{
	//				m_MonsterAIType = MonsterAI::Attack;
	//				m_AIProtocol = MA_ATTACK;

	//				m_fTracePacketTime = 0.f;
	//				m_bMove = false;

	//				// 공격으로 전환하는 상황이기 때문에 클라이언트에게 공격하라는 패킷을 보내준다.
	//				CStream stream;
	//				char packet[MAX_BUFFER_LENGTH] = {};
	//				stream.SetBuffer(packet);
	//				stream.Write(&m_iSerialNumber, sizeof(int));
	//				stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//				//stream.Write(&m_vPos, sizeof(Vector3));

	//				auto	iter = pUserList->begin();
	//				auto	iterEnd = pUserList->end();
	//				for (; iter != iterEnd; ++iter)
	//				{
	//					(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//				}
	//			}
	//			// 클라이언트 몬스터에게 공격하라는 패킷을 보내주도록 한다.
	//		}

	//		else if (fDist <= m_fTraceRange && m_bAttackEnable)
	//		{
	//			//std::cout << "Trace : " << fDist << std::endl;
	//			m_bAttackEnable = true;
	//			m_fAttackTime = 0.f;
	//			m_MonsterAIType = MonsterAI::Trace;

	//			// 타겟의 위치가 변경되었는지 판단한다.
	//			if (m_TargetPos.x != vTargetPos.x || m_TargetPos.y != vTargetPos.y /*|| m_TargetPos.z != vTargetPos.z*/)
	//			{
	//				m_TargetPos = vTargetPos;
	//				// AI Server에 길찾기를 요청한다.
	//				CStream	 stream;
	//				
	//				BYTE 	Packet[MAX_BUFFER_LENGTH] = {};

	//				stream.SetBuffer((char*)Packet);

	//				// 현재 찾고자 하는 내비게이션의 이름을 넘겨준다.
	//				int	NavNameLength = m_NavName.length();
	//				stream.Write(&NavNameLength, 4);
	//				stream.Write(m_NavName.c_str(), NavNameLength);
	//				
	//				// 그렇다면 현재 이 몬스터의 

	//				stream.Write(&m_iSerialNumber, 4);
	//				stream.Write(&m_LevelType, sizeof(ELevel));
	//				stream.Write(&m_vPos, 12);
	//				stream.Write(&m_TargetPos, 12);
	//				
	//				if (CGameManager::GetInst()->GetAISession())
	//					CGameManager::GetInst()->GetAISession()->WritePacket(GP_FINDMONSTERPATH, Packet, stream.GetLength());
	//			}
	//		}

	//		else
	//		{
	//			m_PathList.clear();
	//			// 타겟이 벗어났으므로 타겟을 해제한다.

	//			m_TargetPos = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	//			m_pTarget = nullptr;
	//			m_bAttackEnable = true;
	//			m_fAttackTime = 0.f;
	//			m_TargetTime = 0.f;
	//			m_MonsterAIType = MonsterAI::Idle;
	//			m_AIProtocol = MonsterAIProtocol::MA_IDLE;

	//			CStream stream;
	//			char packet[MAX_BUFFER_LENGTH] = {};
	//			stream.SetBuffer(packet);
	//			stream.Write(&m_iSerialNumber, sizeof(int));
	//			stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));

	//			auto	iter = pUserList->begin();
	//			auto	iterEnd = pUserList->end();
	//			for (; iter != iterEnd; ++iter)
	//			{
	//				(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//			}
	//		}
	//	}
	//}

	//switch (m_MonsterAIType)
	//{
	//case MonsterAI::Idle:
	//	m_bMove = false;
	//	m_bAttackEnable = true;
	//	m_fAttackTime = 0.f;
	//	m_fTracePacketTime = 0.f;
	//	m_AIProtocol = MA_IDLE;    
	//	break;
	//case MonsterAI::Trace:
	//	m_bAttackEnable = true;
	//	m_fAttackTime = 0.f;
	//	TargetTrace(fTime);
	//	break;
	//case MonsterAI::Patrol:
	//	m_bAttackEnable = true;
	//	Patrol(fTime);
	//	break;
	//case MonsterAI::Attack:
	//	m_bMove = false;
	//	m_fTracePacketTime = 0.f;
	//	Attack(fTime);
	//	break;
	//case MonsterAI::Skill1:
	//	Skill1();
	//	break;
	//case MonsterAI::Skill2:
	//	Skill2();
	//	break;
	//case MonsterAI::Death:
	//	m_bAttackEnable = false;
	//	m_fAttackTime = 0.f;
	//	m_fTracePacketTime = 0.f;
	//	m_bMove = false;
	//	break;
	//}

	//if (m_bMove)
	//{
	//	//유저들에게 패킷을 보내준다.
	//	//초당 10번 정도로 패킷을 보내서 이동을 맞춰준다.
	//}
}

bool CMonster::Damage(int PlayerAttack, class CUser* User)
{
	// 플레이어가 이 몬스터를 공격을 할떄
	int iDamage = PlayerAttack - m_iArmor;

	if (iDamage < 0)
	{
		iDamage = 1;
		cout << "Damage : Miss...." << "  PlayerAttack : " << PlayerAttack << "  MonsterArmor : " << m_iArmor << endl;
	}
	else
		cout << "Damage : " << iDamage << "  PlayerAttack : " << PlayerAttack << "  MonsterArmor : " << m_iArmor << endl;

	//m_iHP -= iDamage; 아머가 100이고 플레이어공격이 30이라 데미지 안들어감... 

	//iDamage = 100;// 수정해야함

	m_iHP -= iDamage;

	// Damage 를 이용하여 어그로 수치를 만들어낸다.

	const list<class CUser*>* pUserList = m_pLevel->GetUserList();
	// 모든 유저에게 보내는것 보단 현재 영역에 있는 오브젝트들에게 보내는게 좋지않을까하는데
	// 그런데 현재 몬스터는 영역별로 관리하는게 아니라 레벨별로 관리를 현재 하고 있으므로
	if (m_iHP > 0)
	{
		AddAggroPoint(User, iDamage);

		CStream stream;
		char packet[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(packet);
		stream.Write(&m_iSerialNumber, sizeof(int));
		stream.Write(&m_iHP, sizeof(int));

		auto	iter = pUserList->begin();
		auto	iterEnd = pUserList->end();
		for (; iter != iterEnd; ++iter)
		{
			(*iter)->WritePacket(GP_PLAYERATTACKDAMAGE, (BYTE*)packet, stream.GetLength());
		}
	}
	else if (m_iHP <= 0)
	{
		// 여기서는 죽인 플레이어만 경험치와 골드, 그리고 아이템를 수급하게끔 해야한다.
		// 그리고 파티가 아니라면 처치한 사람에게 주거나 
		// 파티면 그 만큼 나눠서 줘야된다는건데
		m_MonsterAIType = AI_Type::Death;
		m_bAttackEnable = false;
		m_iHP = 0;

		cout << m_iSerialNumber << " " << m_eMonsterType << " - Death....." << endl;

		CStream stream;
		char packet[MAX_BUFFER_LENGTH] = {};
		stream.SetBuffer(packet);
		stream.Write(&m_iSerialNumber, sizeof(int));

		if (User->IsGroup())
		{
			// 파티그룹이라면 파티그룹만 1 / n빵 해줘야한다.
			// 그리고 해당 영역에 있는 파티그룹만 보내줘야한다. 
			// 만약 파티원중 한명이 마을에 있거나 다른 사냥지역에
			// 있으면 그 사람한테도 경험치와 골드를 줄수는 없기때문
			vector<CUser*>* vecGroupList = User->GetGroupList();
			int iGroupCount = (int)vecGroupList->size();
			int iGroupExp = m_iExp / iGroupCount;
			int iGroupGold = m_iGold / iGroupCount;
			// 그룹 체크를 어떻게 해야하지?
		}

		auto	iter = pUserList->begin();
		auto	iterEnd = pUserList->end();
		for (; iter != iterEnd; ++iter)
		{
			if (*iter == User)
			{
				continue;
			}
			(*iter)->WritePacket(GP_MONSTERDEATH, (BYTE*)packet, stream.GetLength());
		}

		ItemDrop(User);

		CStream stream2;
		char packet2[MAX_BUFFER_LENGTH] = {};
		stream2.SetBuffer(packet2);

		stream2.Write(&m_iSerialNumber, sizeof(int));
		stream2.Write(&m_iExp, sizeof(int));
		stream2.Write(&m_iGold, sizeof(int));

		User->WritePacket(GP_MONSTERDEATH, (BYTE*)packet2, stream2.GetLength());
		
		m_pSpawnPoint->MonsterDeath();

		SetDeath(true);

		return true;
	}

	return false;
}

bool CMonster::ItemDrop(class CUser* User)
{
	// 현재 몬스터의 타입을 알고있으니 여기서 몬스터 종류별로 드랍확률이나 드랍아이템 종류를 정해서 보내느게 좋을거 같다.
	// 그리고 인벤토리에다가 넣어주기 전에 유저가 어떤식으로 DropItem을 처리하는지에 따라 방식이 틀려짐.

	const vector<_tagItemInfo*>* pItemList = GET_SINGLE(CGameManager)->GetStoreInfo();
	vector<int> vecItemNumber;
	int iAll_ItemSize = pItemList->size();
	int iRandomNumber = 0;

	random_device rd;
	default_random_engine gen(rd());

	uniform_int_distribution<int> dis(1, 3);
	int iDropItemCount = dis(gen);

	vecItemNumber.reserve(iDropItemCount);

	if (iAll_ItemSize < 1)
	{
		return false;
	}
	
	CStream stream;
	char packet[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(packet);

	int iMonsterNumber = m_iSerialNumber;

	stream.Write(&iMonsterNumber, sizeof(int));

	stream.Write(&iDropItemCount, sizeof(int));

	for (int i = 0; i < iDropItemCount; ++i)
	{
		// 물약 제외
		uniform_int_distribution<int> dis2(2, iAll_ItemSize - 1);
		iRandomNumber = dis2(gen);

		stream.Write((*pItemList)[iRandomNumber]->strName, ITEMNAME_SIZE);
		stream.Write<EItemGrade>(&(*pItemList)[iRandomNumber]->Grade, sizeof(BYTE));
		stream.Write<ItemType>(&(*pItemList)[iRandomNumber]->Type, sizeof(BYTE));
		stream.Write<int>(&(*pItemList)[iRandomNumber]->Price, sizeof(int));
		stream.Write<int>(&(*pItemList)[iRandomNumber]->Sell, sizeof(int));

		int iOptionCount = (int)(*pItemList)[iRandomNumber]->vecOption.size();
		stream.Write<int>(&iOptionCount, sizeof(int));
		for (int i = 0; i < iOptionCount; i++)
		{
			stream.Write<ItemOptionType>(&(*pItemList)[iRandomNumber]->vecOption[i].Type, sizeof(ItemOptionType));
			stream.Write<int>(&(*pItemList)[iRandomNumber]->vecOption[i].Option, sizeof(int));
		}
	}

	User->WritePacket(GP_MONSTERITEMDROP, (BYTE*)packet, stream.GetLength());

	return true;
}

void CMonster::TargetTrace(float fTime)
{
	//const list<class CUser*>* pUserList = m_Level->GetUserList();

	//// 여기서 타겟을 넣어준 이유 : 
	//if (!m_pTarget)
	//{
	//	m_PathList.clear();
	//	m_AIProtocol = MA_IDLE;
	//	return;z
	//}

	//// 근데 이 예외처리를 
	//// 경로가 없을 경우 예외처리를 해주어야 한다.
	//else if (m_PathList.empty())
	//{
	//	// Trace End를 보내준다.
	//	/*m_AIProtocol = MA_TRACEEND;

	//	m_fTracePacketTime = 0.f;

	//	CStream stream;
	//	char packet[MAX_BUFFER_LENGTH] = {};
	//	stream.SetBuffer(packet);
	//	stream.Write(&m_iSerialNumber, sizeof(int));
	//	stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//	stream.Write(&m_vPos, sizeof(Vector3));

	//	auto	iter = pUserList->begin();
	//	auto	iterEnd = pUserList->end();
	//	for (; iter != iterEnd; ++iter)
	//	{
	//		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//	}
	//	m_AIProtocol = MA_IDLE;
	//	m_pTarget = nullptr;*/

	//	return;
	//}

	//// ●여기서 구해놓은 길을 찾아서 이동하도록 처리한다.
	//// 해당 위치로 도착하기전에 다음 위치를 알려줘야 하는데 .
	//// 이동할 위치를 얻어온다.
	//Vector3	TargetPos = m_PathList.front();
	//Vector3	Pos = m_vPos;

	//// 일단 플레이어의 Pos는 우리가 설정한 좌표는 캐릭터 가운데이기 떄문에
	//// 그에 반해 몬스터는 z가 0이기 떄문에 
	//// 플레이어와 몬스터의 거리에서 z가 -값이 될수가 있다.
	//TargetPos.z = 0.f;
	//Pos.z = 0.f;

	//// 이동할 위치로의 방향을 구한다.
	//Vector3	Dir = TargetPos - Pos;
	//float	TargetDist = Dir.Length();

	//Dir.Normalize();

	//float	MoveDist = m_fMoveSpeed * fTime;

	//// 만약 서버의 과부화나 일시적인 트래픽오류로 인해 목적지보다 더 이동했을경우
	//if (MoveDist > TargetDist)
	//{
	//	MoveDist = TargetDist;

	//	m_PathList.pop_front();
	//}

	//m_vPos += Dir * MoveDist;

	//m_vRot = Dir;

	//m_bMove = true;

	//Pos = m_vPos;
	//Pos.z = m_pTarget->GetPos().z;

	//float	fDist = m_pTarget->GetPos().Distance(Pos); // 이동후 거리차이

	//if (m_AIProtocol != MA_TRACESTART && m_AIProtocol != MA_TRACE && m_AIProtocol != MA_TRACEEND /*&& m_AIProtocol != MA_ATTACK*/)
	//{
	//	m_AIProtocol = MA_TRACESTART;
	//}

	//else if (m_AIProtocol == MA_TRACESTART)
	//{
	//	m_AIProtocol = MA_TRACE;
	//}

	//else if (/*(fDist <= m_fAttackRange) ||*/ fDist > m_fTraceRange)
	//{
	//	m_AIProtocol = MA_TRACEEND;
	//}

	//switch (m_AIProtocol)
	//{
	//case MA_IDLE:
	//	break;
	//case MA_TRACESTART:
	//{
	//	CStream stream;
	//	char packet[MAX_BUFFER_LENGTH] = {};
	//	stream.SetBuffer(packet);

	//	stream.Write(&m_iSerialNumber, sizeof(int));
	//	stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//	stream.Write(&m_vRot, sizeof(Vector3));
	//	stream.Write(&m_vPos, sizeof(Vector3));

	//	auto	iter = pUserList->begin();
	//	auto	iterEnd = pUserList->end();
	//	for (; iter != iterEnd; ++iter)
	//	{
	//		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//	}

	//	m_fTracePacketTime = 0.f;
	//}
	//break;
	//case MA_TRACE:
	//{
	//	m_fTracePacketTime += fTime;

	//	if (m_fTracePacketTime >= m_fTracePacketTimeMax)
	//	{
	//		m_fTracePacketTime -= m_fTracePacketTimeMax;

	//		CStream stream;
	//		char packet[MAX_BUFFER_LENGTH] = {};
	//		stream.SetBuffer(packet);
	//		stream.Write(&m_iSerialNumber, sizeof(int));
	//		stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//		stream.Write(&m_vRot, sizeof(Vector3));
	//		stream.Write(&m_vPos, sizeof(Vector3));

	//		auto	iter = pUserList->begin();
	//		auto	iterEnd = pUserList->end();
	//		for (; iter != iterEnd; ++iter)
	//		{
	//			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//		}
	//	}
	//}
	//break;
	//case MA_TRACEEND:
	//{
	//	m_fTracePacketTime = 0.f;

	//	CStream stream;
	//	char packet[MAX_BUFFER_LENGTH] = {};
	//	stream.SetBuffer(packet);
	//	stream.Write(&m_iSerialNumber, sizeof(int));
	//	stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//	stream.Write(&m_vPos, sizeof(Vector3));

	//	auto	iter = pUserList->begin();
	//	auto	iterEnd = pUserList->end();
	//	for (; iter != iterEnd; ++iter)
	//	{
	//		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//	}
	//	m_AIProtocol = MA_IDLE;
	//	m_pTarget = nullptr;
	//}
	//break;
	//}


	// -수정전 Trace-
	
	//이동 방향을 구한다. 
	//Vector3	vTargetPos = m_pTarget->GetPos();
	//Vector3	vPos = m_vPos;
	////vPos.z = 0.f;
	////vTargetPos.z = 0.f;
	////std::cout << "이동처리중" << std::endl;
	//Vector3	vDir = vTargetPos - vPos;
	//vDir.Normalize();
	//m_vPos += vDir * m_fMoveSpeed * fTime;
	//m_vDir = vDir;
	//Vector3 vPrePos = vTargetPos;
	//float	vDist = m_vPos.Distance(vPrePos);
	//if (vDist > 175.f)
	//	m_vPos = vPrePos;
	//m_bMove = true;
	//vPos = m_vPos;
	//vPos.z = 0.f;
	//float	fDist = vTargetPos.Distance(vPos); // 이동후 거리차이
	//if (m_AIProtocol != MA_TRACESTART && m_AIProtocol != MA_TRACE && m_AIProtocol != MA_TRACEEND)
	//{
	//	m_AIProtocol = MA_TRACESTART;
	//}
	//else if (m_AIProtocol == MA_TRACESTART)
	//{
	//	m_AIProtocol = MA_TRACE;
	//}
	//else if (/*(fDist <= m_fAttackRange) ||*/ fDist > m_fTraceRange)
	//{
	//	m_AIProtocol = MA_TRACEEND;
	//}
	//-------------------------------------------------
	//switch (m_AIProtocol)
	//{
	//case MA_IDLE:
	//	break;
	//case MA_TRACESTART:
	//{
	//	std::cout << "Trace Start " << std::endl;
	//	CStream stream;
	//	char packet[MAX_BUFFER_LENGTH] = {};
	//	stream.SetBuffer(packet);
	//	stream.Write(&m_iSerialNumber, sizeof(int));
	//	stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//	stream.Write(&vDir, sizeof(Vector3));
	//	stream.Write(&m_vPos, sizeof(Vector3));
	//	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	//	auto	iter = pUserList->begin();
	//	auto	iterEnd = pUserList->end();
	//	for (; iter != iterEnd; ++iter)
	//	{
	//		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//	}
	//	m_fTracePacketTime = 0.f;
	//}
	//break;
	//case MA_TRACE:
	//{
	//	m_fTracePacketTime += fTime;
	//	if (m_fTracePacketTime >= m_fTracePacketTimeMax)
	//	{
	//		m_fTracePacketTime -= m_fTracePacketTimeMax;
	//		CStream stream;
	//		char packet[MAX_BUFFER_LENGTH] = {};
	//		stream.SetBuffer(packet);
	//		stream.Write(&m_iSerialNumber, sizeof(int));
	//		stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//		stream.Write(&vDir, sizeof(Vector3));
	//		stream.Write(&m_vPos, sizeof(Vector3));
	//		const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	//		auto	iter = pUserList->begin();
	//		auto	iterEnd = pUserList->end();
	//		for (; iter != iterEnd; ++iter)
	//		{	
	//			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//		}
	//	}
	//}
	//break;
	//case MA_TRACEEND:
	//{
	//	std::cout << "Trace End" << std::endl;
	//	m_fTracePacketTime = 0.f;
	//	CStream stream;
	//	char packet[MAX_BUFFER_LENGTH] = {};
	//	stream.SetBuffer(packet);
	//	stream.Write(&m_iSerialNumber, sizeof(int));
	//	stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));
	//	stream.Write(&m_vPos, sizeof(Vector3));
	//	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	//	auto	iter = pUserList->begin();
	//	auto	iterEnd = pUserList->end();
	//	for (; iter != iterEnd; ++iter)
	//	{
	//		(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//	}
	//	m_AIProtocol = MA_IDLE;
	//	m_pTarget = nullptr;
	//}
	//break;
	//}
}

void CMonster::Patrol(float fTime)
{
	/*if (m_pTarget)
	{
		return;
	}

	switch (m_AIProtocol)
	{
	case MA_PATROLSTART:
		break;
	case MA_PATROL:
		break;
	case MA_PATROLEND:
		break;
	}*/
}

void CMonster::Attack(float fTime)
{
	//m_fAttackTime += fTime;

	//const list<class CUser*>* pUserList = m_Level->GetUserList();
	///*
	//example
	// 
	//m_fAttackTime = 0.f;
	//m_fAttackEnableTime = 0.286f;
	//m_fAttackEnd = 1.17f;
	//m_bAttackEnable = false;
	//*/
	//if (m_bAttackEnable)
	//{
	//	if (m_fAttackTime >= m_fAttackEnableTime)
	//	{
	//		m_bAttackEnable = false;
	//		m_AIProtocol = MA_DAMAGE;
	//		// 클라이언트 몬스터에게 공격하라는 패킷을 보내주도록 한다.
	//		CStream stream;
	//		char packet[MAX_BUFFER_LENGTH] = {};
	//		stream.SetBuffer(packet);
	//		stream.Write(&m_iSerialNumber, sizeof(int));
	//		stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));

	//		int	iDamage = m_iAttack - m_pTarget->GetArmor();
	//		iDamage = iDamage < 1 ? 1 : iDamage;

	//		iDamage = 10; // 없애자

	//		stream.Write(&iDamage, sizeof(int));

	//		m_pTarget->AddHP(iDamage);

	//		int	iHP = m_pTarget->GetHP();
	//		stream.Write(&iHP, sizeof(int));

	//		int	iChacterNumber = m_pTarget->GetCharacterNumber();
	//		stream.Write(&iChacterNumber, sizeof(int));

	//		auto	iter = pUserList->begin();
	//		auto	iterEnd = pUserList->end();
	//		for (; iter != iterEnd; ++iter)
	//		{
	//			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//		}
	//	}
	//}

	//else
	//{
	//	if (m_fAttackTime >= m_fAttackEnd)
	//	{
	//		m_bAttackEnable = true;
	//		m_fAttackTime -= m_fAttackEnd;

	//		Vector3	vTargetPos = m_pTarget->GetPos();
	//		Vector3	vPos = m_vPos;
	//		vPos.z = 0.f;
	//		vTargetPos.z = 0.f;

	//		Vector3	vDir = vTargetPos - vPos;
	//		vDir.Normalize();
	//		m_vRot = vDir;

	//		m_AIProtocol = MA_ATTACKEND;
	//		// 클라이언트 몬스터에게 공격하라는 패킷을 보내주도록 한다.

	//		CStream stream;
	//		char packet[MAX_BUFFER_LENGTH] = {};
	//		stream.SetBuffer(packet);
	//		stream.Write(&m_iSerialNumber, sizeof(int));
	//		stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));

	//		stream.Write(&vDir, sizeof(Vector3));

	//		auto	iter = pUserList->begin();
	//		auto	iterEnd = pUserList->end();
	//		for (; iter != iterEnd; ++iter)
	//		{
	//			(*iter)->WritePacket(GP_MONSTERAI, (BYTE*)packet, stream.GetLength());
	//		}
	//	}
	//}
}

void CMonster::Skill1()
{
}

void CMonster::Skill2()
{
}

int CMonster::AddPacket(BYTE* pPacket, int iOffset)
{
	int	iSize = 0;

	memcpy(pPacket + (iOffset + iSize), &m_eMonsterType, sizeof(Monster_Type));
	iSize += sizeof(Monster_Type);

	memcpy(pPacket + (iOffset + iSize), &m_vPos, sizeof(Vector3));
	iSize += sizeof(Vector3);

	memcpy(pPacket + (iOffset + iSize), &m_vScale, sizeof(Vector3));
	iSize += sizeof(Vector3);

	memcpy(pPacket + (iOffset + iSize), &m_vRot, sizeof(Vector3));
	iSize += sizeof(Vector3);

	/*
	memcpy(pPacket + (iOffset + iSize), &m_iAttack, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iArmor, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iHP, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iHPMax, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iMP, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iMPMax, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iLevel, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iExp, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_iGold, sizeof(int));
	iSize += sizeof(int);

	memcpy(pPacket + (iOffset + iSize), &m_fMoveSpeed, sizeof(float));
	iSize += sizeof(float);

	memcpy(pPacket + (iOffset + iSize), &m_fAttackSpeed, sizeof(float));
	iSize += sizeof(float);

	memcpy(pPacket + (iOffset + iSize), &m_fTraceRange, sizeof(float));
	iSize += sizeof(float);

	memcpy(pPacket + (iOffset + iSize), &m_fAttackRange, sizeof(float));
	iSize += sizeof(float);*/

	memcpy(pPacket + (iOffset + iSize), &m_iSerialNumber, sizeof(int));
	iSize += sizeof(int);

	return iSize;
}

void CMonster::Copy(CMonster* Monster)
{
	CNPC::Copy(Monster);

	m_iSerialNumber = Monster->m_iSerialNumber;
	m_eMonsterType = Monster->m_eMonsterType;
}
