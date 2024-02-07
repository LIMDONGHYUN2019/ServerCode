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

	//// ���⼭�� ��� ���͵��� �������� ����� �� �� �ְ� ���ݴ���� ã���ִ� �۾��� �����ϰ� ���� ��Ŷ�� ���� Ŭ�������� ������ ��
	//// �ֵ��� �Ѵ�.

	///*
	//	Ÿ���� ������ ����� �翬�� ��θ� �ٽ� ����� ���־�� �Ұ��̴�.
	//	������ ���� Ÿ���� ���� �� �ȿ����� �������� ����� ��, �ٸ� ���� �̵��� �� ���� �ƴ϶�� ��θ� �ٽ� ã�� �ʿ�� ���� ���̴�.
	//	Ÿ���� ��ġ�� ���ڸ��� �ְų� Ȥ�� �ٸ� ���� �̵��� �ߴ����� �Ǵ����־�� �Ѵ�.
	//*/
	//if (m_fDetectTime >= m_fDetectTimeMax)
	//{
	//	m_fDetectTime -= m_fDetectTimeMax;
	//	// 1�ʿ� 10���� Ž���Ѵ�.
	//	if (m_pTarget)
	//	{
	//		// ���� Ÿ���� �׾����� �Ǵ��Ѵ�.
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
	//		// ���� ����� ������ Ÿ������ ã���ش�.
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

	//				//if(fDist < m_fTargetDistance) // �Ƹ��� ���� ���Ϳ��� �Ÿ��� ������ �η��µ�.
	//				//{
	//				//	m_pTarget = *iter;
	//				//	m_fTargetDistance = fDist;
	//				//}
	//				m_pTarget = *iter;
	//				m_fTargetDistance = fDist;

	//				m_TargetPos = vPos;
	//				// �i�ư����� Ÿ���� �̵����������� ���� ã���ʿ䰡������ Ÿ���� ������ġ�� �޾ƿ°�.

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

	//		vPos.z = vTargetPos.z;//���� ������ҽ� ���������� ���Ͱ� ���ݽ� ������ ���� ��ġ�� Ʋ�������ִ�.

	//		float	fDist = vPos.Distance(vTargetPos);
	//		//vDir.z = 0.f;//����
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

	//				// �������� ��ȯ�ϴ� ��Ȳ�̱� ������ Ŭ���̾�Ʈ���� �����϶�� ��Ŷ�� �����ش�.
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
	//			// Ŭ���̾�Ʈ ���Ϳ��� �����϶�� ��Ŷ�� �����ֵ��� �Ѵ�.
	//		}

	//		else if (fDist <= m_fTraceRange && m_bAttackEnable)
	//		{
	//			//std::cout << "Trace : " << fDist << std::endl;
	//			m_bAttackEnable = true;
	//			m_fAttackTime = 0.f;
	//			m_MonsterAIType = MonsterAI::Trace;

	//			// Ÿ���� ��ġ�� ����Ǿ����� �Ǵ��Ѵ�.
	//			if (m_TargetPos.x != vTargetPos.x || m_TargetPos.y != vTargetPos.y /*|| m_TargetPos.z != vTargetPos.z*/)
	//			{
	//				m_TargetPos = vTargetPos;
	//				// AI Server�� ��ã�⸦ ��û�Ѵ�.
	//				CStream	 stream;
	//				
	//				BYTE 	Packet[MAX_BUFFER_LENGTH] = {};

	//				stream.SetBuffer((char*)Packet);

	//				// ���� ã���� �ϴ� ������̼��� �̸��� �Ѱ��ش�.
	//				int	NavNameLength = m_NavName.length();
	//				stream.Write(&NavNameLength, 4);
	//				stream.Write(m_NavName.c_str(), NavNameLength);
	//				
	//				// �׷��ٸ� ���� �� ������ 

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
	//			// Ÿ���� ������Ƿ� Ÿ���� �����Ѵ�.

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
	//	//�����鿡�� ��Ŷ�� �����ش�.
	//	//�ʴ� 10�� ������ ��Ŷ�� ������ �̵��� �����ش�.
	//}
}

bool CMonster::Damage(int PlayerAttack, class CUser* User)
{
	// �÷��̾ �� ���͸� ������ �ҋ�
	int iDamage = PlayerAttack - m_iArmor;

	if (iDamage < 0)
	{
		iDamage = 1;
		cout << "Damage : Miss...." << "  PlayerAttack : " << PlayerAttack << "  MonsterArmor : " << m_iArmor << endl;
	}
	else
		cout << "Damage : " << iDamage << "  PlayerAttack : " << PlayerAttack << "  MonsterArmor : " << m_iArmor << endl;

	//m_iHP -= iDamage; �ƸӰ� 100�̰� �÷��̾������ 30�̶� ������ �ȵ�... 

	//iDamage = 100;// �����ؾ���

	m_iHP -= iDamage;

	// Damage �� �̿��Ͽ� ��׷� ��ġ�� ������.

	const list<class CUser*>* pUserList = m_pLevel->GetUserList();
	// ��� �������� �����°� ���� ���� ������ �ִ� ������Ʈ�鿡�� �����°� �����������ϴµ�
	// �׷��� ���� ���ʹ� �������� �����ϴ°� �ƴ϶� �������� ������ ���� �ϰ� �����Ƿ�
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
		// ���⼭�� ���� �÷��̾ ����ġ�� ���, �׸��� �����۸� �����ϰԲ� �ؾ��Ѵ�.
		// �׸��� ��Ƽ�� �ƴ϶�� óġ�� ������� �ְų� 
		// ��Ƽ�� �� ��ŭ ������ ��ߵȴٴ°ǵ�
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
			// ��Ƽ�׷��̶�� ��Ƽ�׷츸 1 / n�� ������Ѵ�.
			// �׸��� �ش� ������ �ִ� ��Ƽ�׷츸 ��������Ѵ�. 
			// ���� ��Ƽ���� �Ѹ��� ������ �ְų� �ٸ� ���������
			// ������ �� ������׵� ����ġ�� ��带 �ټ��� ���⶧��
			vector<CUser*>* vecGroupList = User->GetGroupList();
			int iGroupCount = (int)vecGroupList->size();
			int iGroupExp = m_iExp / iGroupCount;
			int iGroupGold = m_iGold / iGroupCount;
			// �׷� üũ�� ��� �ؾ�����?
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
	// ���� ������ Ÿ���� �˰������� ���⼭ ���� �������� ���Ȯ���̳� ��������� ������ ���ؼ� �������� ������ ����.
	// �׸��� �κ��丮���ٰ� �־��ֱ� ���� ������ ������� DropItem�� ó���ϴ����� ���� ����� Ʋ����.

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
		// ���� ����
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

	//// ���⼭ Ÿ���� �־��� ���� : 
	//if (!m_pTarget)
	//{
	//	m_PathList.clear();
	//	m_AIProtocol = MA_IDLE;
	//	return;z
	//}

	//// �ٵ� �� ����ó���� 
	//// ��ΰ� ���� ��� ����ó���� ���־�� �Ѵ�.
	//else if (m_PathList.empty())
	//{
	//	// Trace End�� �����ش�.
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

	//// �ܿ��⼭ ���س��� ���� ã�Ƽ� �̵��ϵ��� ó���Ѵ�.
	//// �ش� ��ġ�� �����ϱ����� ���� ��ġ�� �˷���� �ϴµ� .
	//// �̵��� ��ġ�� ���´�.
	//Vector3	TargetPos = m_PathList.front();
	//Vector3	Pos = m_vPos;

	//// �ϴ� �÷��̾��� Pos�� �츮�� ������ ��ǥ�� ĳ���� ����̱� ������
	//// �׿� ���� ���ʹ� z�� 0�̱� ������ 
	//// �÷��̾�� ������ �Ÿ����� z�� -���� �ɼ��� �ִ�.
	//TargetPos.z = 0.f;
	//Pos.z = 0.f;

	//// �̵��� ��ġ���� ������ ���Ѵ�.
	//Vector3	Dir = TargetPos - Pos;
	//float	TargetDist = Dir.Length();

	//Dir.Normalize();

	//float	MoveDist = m_fMoveSpeed * fTime;

	//// ���� ������ ����ȭ�� �Ͻ����� Ʈ���ȿ����� ���� ���������� �� �̵��������
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

	//float	fDist = m_pTarget->GetPos().Distance(Pos); // �̵��� �Ÿ�����

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


	// -������ Trace-
	
	//�̵� ������ ���Ѵ�. 
	//Vector3	vTargetPos = m_pTarget->GetPos();
	//Vector3	vPos = m_vPos;
	////vPos.z = 0.f;
	////vTargetPos.z = 0.f;
	////std::cout << "�̵�ó����" << std::endl;
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
	//float	fDist = vTargetPos.Distance(vPos); // �̵��� �Ÿ�����
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
	//		// Ŭ���̾�Ʈ ���Ϳ��� �����϶�� ��Ŷ�� �����ֵ��� �Ѵ�.
	//		CStream stream;
	//		char packet[MAX_BUFFER_LENGTH] = {};
	//		stream.SetBuffer(packet);
	//		stream.Write(&m_iSerialNumber, sizeof(int));
	//		stream.Write(&m_AIProtocol, sizeof(MonsterAIProtocol));

	//		int	iDamage = m_iAttack - m_pTarget->GetArmor();
	//		iDamage = iDamage < 1 ? 1 : iDamage;

	//		iDamage = 10; // ������

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
	//		// Ŭ���̾�Ʈ ���Ϳ��� �����϶�� ��Ŷ�� �����ֵ��� �Ѵ�.

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
