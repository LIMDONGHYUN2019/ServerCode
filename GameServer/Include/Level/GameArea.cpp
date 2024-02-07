#include "GameArea.h"
#include "../User.h"
//#include "../GameAreaManager.h"
#include "Stream.h"
#include <iostream>
#include "Level.h"

CGameArea::CGameArea()
{
}

CGameArea::~CGameArea()
{
	SAFE_DELETE_VECLIST(m_UserList);
}

bool CGameArea::Init(const Vector3& vMin, const Vector3& vMax, int idxH, int idxV, int idx)
{
	m_vMin = vMin;
	m_vMax = vMax;
	m_iIndexH = idxH; // y
	m_iIndexV = idxV; // x
	m_iIndex = idx;

	return true;
}

void CGameArea::AddUser(CUser* pUser)
{
	//pUser->SetAreaIndex(m_iIndexH, m_iIndexV, m_iIndex, this);
	// 해당 영역에 넣어줌 누구누구있는지 체크
	m_UserList.push_back(pUser);
}

void CGameArea::DeleteUser(CUser* pUser)
{
	m_UserList.remove(pUser);
	pUser->ClearArea();
}

int CGameArea::UserAppear(int* UserCount, CUser* pUser, char OtherSendPacket[], int OtherPacketLength)
{
	if (m_UserList.empty())
		return OtherPacketLength;

	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	int iCharacterNumber = pUser->GetCharacterNumber();
	
	Vector3 vPos = pUser->GetPos();
	Vector3 vScale = pUser->GetScale();
	Vector3 vRot = pUser->GetRotation();
	stream1.Write<int>(&iCharacterNumber, sizeof(int));
	const char* pJob = pUser->GetJobName();
	const char* pName = pUser->GetstrName();
	_tagCharacterInfo	tCharInfo = pUser->GetChracterInfo();
	int iJob = -1;

	if (strcmp(pJob, "GreyStone") == 0)
		pUser->SetiJob(0);
	else if (strcmp(pJob, "TwinBlast") == 0)
		pUser->SetiJob(1);
	else if (strcmp(pJob, "Serath") == 0)
		pUser->SetiJob(2);
	iJob = pUser->GetiJob();
	stream1.Write<int>(&iJob, sizeof(int));
	// 위치정보
	stream1.Write<Vector3>(&vPos, sizeof(Vector3));
	// 크기정보
	stream1.Write<Vector3>(&vScale, sizeof(Vector3));
	// 회전정보
	stream1.Write<Vector3>(&vRot, sizeof(Vector3));
	stream1.Write<_tagCharacterInfo>(&tCharInfo, sizeof(_tagCharacterInfo));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	//int iOtherUserCount = (int)m_UserList.size() - 1;
	//memcpy(OtherSendPacket + OtherPacketLength, &iOtherUserCount, sizeof(int));
	//OtherPacketLength += sizeof(int);

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;
		//std::cout << (*iter) << "   User Send" << std::endl;

		(*iter)->WritePacket(GP_USERAPPEAR, (BYTE*)strPacket, stream1.GetLength());
		// 나를 주변에 다른유저에게 정보를 보낸다.
		// =================================================================================
		CUser* Tempo = *iter;
		const char* pName= Tempo->GetstrName();
		_tagCharacterInfo tOtherCharInfo = Tempo->GetChracterInfo();
		int iCharacterNumber = Tempo->GetCharacterNumber();
		int iJob = Tempo->GetiJob();
		Vector3 vPos = Tempo->GetPos();
		Vector3 vScale = Tempo->GetScale();
		Vector3 vRot = Tempo->GetRotation();

		memcpy(OtherSendPacket + OtherPacketLength, &iCharacterNumber, sizeof(int));
		OtherPacketLength += sizeof(int);

		memcpy(OtherSendPacket + OtherPacketLength, &iJob, sizeof(int));
		OtherPacketLength += sizeof(int);

		memcpy(OtherSendPacket + OtherPacketLength, &vPos, sizeof(Vector3));
		OtherPacketLength += sizeof(Vector3);

		memcpy(OtherSendPacket + OtherPacketLength, &vScale, sizeof(Vector3));
		OtherPacketLength += sizeof(Vector3);

		memcpy(OtherSendPacket + OtherPacketLength, &vRot, sizeof(Vector3));
		OtherPacketLength += sizeof(Vector3);

		memcpy(OtherSendPacket + OtherPacketLength, &tOtherCharInfo, sizeof(_tagCharacterInfo));
		OtherPacketLength += sizeof(_tagCharacterInfo);

		++(*UserCount);

		if (*UserCount == 10)
		{
			int	MaxCount = 10;

			memcpy(OtherSendPacket, &MaxCount, 4);

			pUser->WritePacket(GP_OTHERUSERAPPEAR, (const BYTE*)OtherSendPacket, OtherPacketLength);

			memset(OtherSendPacket, 0, OtherPacketLength);

			*UserCount = 0;

			OtherPacketLength = 4;
		}

		// 다른유저의 정보들을 모아 나한테 보내 다른유저를 스폰 시키기위해.
	}

	return OtherPacketLength;
}

int CGameArea::UserDisAppear(int* UserCount, class CUser* pUser, char OtherSendPacket[], int OtherPacketLength)
{
	if (m_UserList.empty())
		return OtherPacketLength;

	CStream	stream;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(strPacket);

	int iPlayerNumber = pUser->GetCharacterNumber();
	stream.Write<int>(&iPlayerNumber, sizeof(int));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();
	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		// 다른 유저화면에 내 모습을 없애는것. 
		(*iter)->WritePacket(GP_USERDISAPPEAR, (BYTE*)strPacket, stream.GetLength());
		// =================================================================================
		CUser* Tempo = *iter;
		int iOtherUserNumber = Tempo->GetCharacterNumber();
		memcpy(OtherSendPacket + OtherPacketLength, &iOtherUserNumber, sizeof(int));
		OtherPacketLength += sizeof(int);

		(*UserCount)++;

		if (*UserCount == 10)
		{
			int	MaxCount = 10;

			memcpy(OtherSendPacket, &MaxCount, 4);

			pUser->WritePacket(GP_OTHERUSERDISAPPEAR, (const BYTE*)OtherSendPacket, OtherPacketLength);

			memset(OtherSendPacket, 0, OtherPacketLength);

			*UserCount = 0;

			OtherPacketLength = 4;
		}
	}

	return OtherPacketLength;
}

void CGameArea::SendMoveStart(CUser* pUser)
{
	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	int iCharacterNumber = pUser->GetCharacterNumber();
	Vector3 vPos = pUser->GetPos();
	Vector3 vRot = pUser->GetRotation();
	Vector3 vDir = pUser->GetMoveDir();
	float fViewDir = pUser->GetViewDir();
	float fMoveSpeed = pUser->GetMoveSpeed();

	// 캐릭터 번호
	stream1.Write<int>(&iCharacterNumber, sizeof(int));
	// 위치정보
	stream1.Write<Vector3>(&vPos, sizeof(Vector3));
	stream1.Write<Vector3>(&vRot, sizeof(Vector3));
	// 이동 방향
	stream1.Write<Vector3>(&vDir, sizeof(Vector3));
	stream1.Write<float>(&fViewDir, sizeof(float));
	stream1.Write<float>(&fMoveSpeed, sizeof(float));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		(*iter)->WritePacket(GP_OTHERMOVESTART, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameArea::SendMove(CUser* pUser)
{
	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	int iCharacterNumber = pUser->GetCharacterNumber();
	Vector3 vPos = pUser->GetPos();
	Vector3 vDir = pUser->GetMoveDir();
	Vector3 vRot = pUser->GetRotation();
	float fViewDir = pUser->GetViewDir();
	float fMoveSpeed = pUser->GetMoveSpeed();

	// 캐릭터 번호
	stream1.Write<int>(&iCharacterNumber, sizeof(int));

	// 위치정보
	stream1.Write<Vector3>(&vPos, sizeof(Vector3));

	stream1.Write<Vector3>(&vRot, sizeof(Vector3));

	// 방향정보
	stream1.Write<Vector3>(&vDir, sizeof(Vector3));

	stream1.Write<float>(&fViewDir, sizeof(float));

	stream1.Write<float>(&fMoveSpeed, sizeof(float));

	// 회전 정보
	//stream1.Write<Vector3>(&vRot, sizeof(Vector3));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		(*iter)->WritePacket(GP_OTHERMOVE, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameArea::SendMoveEnd(CUser* pUser)
{
	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	int iCharacterNumber = pUser->GetCharacterNumber();
	Vector3 vPos = pUser->GetPos();
	//Vector3 vDir = pUser->GetMoveDir();
	Vector3 vRot = pUser->GetRotation();

	// 캐릭터 번호
	stream1.Write<int>(&iCharacterNumber, sizeof(int));

	// 위치정보
	stream1.Write<Vector3>(&vPos, sizeof(Vector3));

	stream1.Write<Vector3>(&vRot, sizeof(Vector3));

	// 방향정보
	//stream1.Write<Vector3>(&vDir, sizeof(Vector3));

	// 회전 정보
	//stream1.Write<Vector3>(&vRot, sizeof(Vector3));


	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		(*iter)->WritePacket(GP_OTHERMOVEEND, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameArea::OtherAttack(CUser* pUser, AttackType tAttack)
{
	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);
	int iCharacterNumber = pUser->GetCharacterNumber();
	stream1.Write<int>(&iCharacterNumber, sizeof(int));
	AttackType AttackType = tAttack;
	stream1.Write(&AttackType, sizeof(bool));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		(*iter)->WritePacket(GP_ATTACK, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameArea::OtherAttack(CUser* pUser, int AttackIndex)
{
	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);
	int iCharacterNumber = pUser->GetCharacterNumber();
	stream1.Write<int>(&iCharacterNumber, sizeof(int));

	AttackType eAttack_Type = AttackType::NormalAttack;
	stream1.Write<AttackType>(&eAttack_Type, sizeof(bool));

	stream1.Write<int>(&AttackIndex, sizeof(int));

	float ViewDir = pUser->GetViewDir();

	stream1.Write<float>(&ViewDir, sizeof(float));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		(*iter)->WritePacket(GP_ATTACK, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameArea::OtherRotation(CUser* pUser, float Yaw)
{
	CStream	stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);
	int iCharacterNumber = pUser->GetCharacterNumber();
	stream1.Write<int>(&iCharacterNumber, sizeof(int));
	//stream1.Write<BYTE>(Packet, Length);

	stream1.Write<float>(&Yaw, sizeof(float));

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		(*iter)->WritePacket(GP_ROTATION, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameArea::MonsterAppear(const char* pMonsterInfo, UINT InfoSize)
{
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	memcpy(strPacket, pMonsterInfo, MAX_BUFFER_LENGTH);

	auto	iter = m_UserList.begin();
	auto	iterEnd = m_UserList.end();

	for (; iter != iterEnd; ++iter)
	{
		(*iter)->WritePacket(GP_MONSTERCREATE, (BYTE*)strPacket, InfoSize);
	}
}

void CGameArea::Update(float fTime)
{
	// 수시로 체크
	//auto	iter = m_UserList.begin();
	//auto	iterEnd = m_UserList.end();

	//for (; iter != iterEnd;)
	//{
	//	if (!(*iter)->GetConnect())
	//	{
	//		// 이 유저는 제거되어야 한다.
	//		// 유저를 제거할때 8방향을 검색하여 8방향의 다른 유저들에게 이 유저를 제거하라고 요청한다.
	//		/*int	StartIndexH = m_iIndexH - 1;
	//		int	EndIndexH = m_iIndexH + 1;

	//		int	StartIndexV = m_iIndexV - 1;
	//		int	EndIndexV = m_iIndexV + 1;

	//		StartIndexH = StartIndexH < 0 ? 0 : StartIndexH;
	//		StartIndexV = StartIndexV < 0 ? 0 : StartIndexV;*/

	//		//EndIndexH = EndIndexH > 0 ? 0 : EndIndexH;
	//		//StartIndexV = StartIndexV < 0 ? 0 : StartIndexV;

	//		iter = m_UserList.erase(iter);
	//		iterEnd = m_UserList.end();
	//		continue;
	//	} // 접속이 끊겼는지

	//	++iter;


	//	// 위치가 바뀐 유저가 있다면 영역을 변경해준다.
	//	//int iIndex = m_Level->GetAreaIndex((*iter)->GetPos());

	//	//if (iIndex != m_iIndex) // 현재 인데스가 이 인덱스와 같지않다면
	//	//{
	//	//	// UserDisAppear((*iter));

	//	//	m_Level->AddAreaUser(*iter);

	//	//	iter = m_UserList.erase(iter);
	//	//}

	//	//else
	//	//	++iter;
	//}
}

