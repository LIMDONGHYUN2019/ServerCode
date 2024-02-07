#pragma once

#include "../GameInfo.h"

class CGameArea // 타일
{
	friend class CLevel;

public:
	CGameArea();
	~CGameArea();


private:
	class CLevel* m_Level;
	list<class CUser*>	m_UserList; // 이 타일에 있는 유저의 정보들
	Vector3	m_vMin;
	Vector3	m_vMax;
	int		m_iIndexH;
	int		m_iIndexV;
	int		m_iIndex;

	char OtherPacket[MAX_BUFFER_LENGTH] = {};

public:
	const Vector3& GetMin()	const
	{
		return m_vMin;
	}

	const Vector3& GetMax()	const
	{
		return m_vMax;
	}

	const list<class CUser*>* GetUserList()	const
	{
		return &m_UserList;
	}

	int GetIndexH()	const
	{
		return m_iIndexH;
	}

	int GetIndexV()	const
	{
		return m_iIndexV;
	}

	int GetIndex()	const
	{
		return m_iIndex; 
	}

public:
	bool Init(const Vector3& vMin, const Vector3& vMax, int idxH, int idxV, int idx);
	void AddUser(class CUser* pUser);
	void DeleteUser(class CUser* pUser);
	int UserAppear(int* UserCount, class CUser* pUser, char OtherSendPacket[], int OtherPacketLength);
	int UserDisAppear(int* UserCount, class CUser* pUser, char OtherSendPacket[], int OtherPacketLength);
	void SendMoveStart(class CUser* pUser);
	void SendMove(class CUser* pUser);
	void SendMoveEnd(class CUser* pUser);
	void OtherAttack(class CUser* pUser, AttackType tAttack);
	void OtherAttack(class CUser* pUser, int AttackIndex);
	void OtherRotation(class CUser* pUser, float Yaw);
	void MonsterAppear(const char* pMonsterInfo, UINT InfoSize);
	void Update(float fTime);
};

