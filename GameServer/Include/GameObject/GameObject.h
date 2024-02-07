#pragma once

#include "../GameInfo.h"
#include "../AI/AIAgent.h"
#include "../Sync.h"

class CGameObject
{
protected:
	CGameObject();
	CGameObject(const CGameObject& obj);
	virtual ~CGameObject();

protected:
	class CLevel*			m_pLevel;
	bool					m_bDeath;
	char					m_strName[NAME_SIZE];
	ELevel					m_eLevelType;
	GameObject_Type			m_eObjType;
	Vector3					m_vPrevPos;
	Vector3					m_vPos;
	Vector3					m_vPrevRot;
	Vector3					m_vRot;
	Vector3					m_vScale;
	Vector3					m_vMoveDir;

public:
	GameObject_Type GetObjType()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_eObjType;
	}

	ELevel GetLevelType()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_eLevelType;
	}

	class CLevel* GetLevel()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_pLevel;
	}

	const Vector3& GetPos()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_vPos;
	}

	const Vector3& GetPrevPos()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_vPrevPos;
	}

	const Vector3& GetScale()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_vScale;
	}

	const Vector3& GetRotation()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_vRot;
	}

	const Vector3& GetPrevRotation()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_vPrevRot;
	}

	const Vector3& GetMoveDir()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_vMoveDir;
	}

	const char* GetName()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_strName;
	}

	bool IsDeath()	const
	{
		CSectionSync	sync(&g_MainCrt);

		return m_bDeath;
	}

public:
	void SetName(const char* Name)
	{
		CSectionSync	sync(&g_MainCrt);

		strcpy_s(m_strName, Name);
	}

	void SetPos(const Vector3& vPos)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPos = vPos;
	}

	void SetPos(float x, float y, float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPos = Vector3(x, y, z);
	}

	void SetPosZ(float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPos.z = z;
	}

	void SetPrevPos(const Vector3& vPos)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevPos = vPos;
	}

	void SetPrevPos(float x, float y, float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevPos = Vector3(x, y, z);
	}

	void SetPrevPosZ(float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevPos.z = z;
	}

	void SetScale(const Vector3& vScale)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vScale = vScale;
	}

	void SetScale(float x, float y, float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vScale = Vector3(x, y, z);
	}

	void SetRotation(const Vector3& Rot)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vRot = Rot;
	}
	void SetRotation(float x, float y, float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vRot = Vector3(x, y, z);
	}

	void SetRotationX(float fX)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vRot.x = fX;
	}

	void SetRotationY(float fY)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vRot.y = fY;
	}

	void SetRotationZ(float fZ)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vRot.z = fZ;
	}

	void SetPrevRotation(const Vector3& Rot)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevRot = Rot;
	}
	void SetPrevRotation(float x, float y, float z)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevRot = Vector3(x, y, z);
	}

	void SetPrevRotationX(float fX)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevRot.x = fX;
	}

	void SetPrevRotationY(float fY)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevRot.y = fY;
	}

	void SetPrevRotationZ(float fZ)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vPrevRot.z = fZ;
	}


	void SetMoveDir(const Vector3& vDir)
	{
		CSectionSync	sync(&g_MainCrt);

		m_vMoveDir = vDir;
	}

	void SetDeath(bool bDeath)
	{
		CSectionSync	sync(&g_MainCrt);

		m_bDeath = bDeath;
	}

public:
	virtual bool Init();
	virtual bool Init(FILE* pFile);
	virtual void Update(float fTime);
	virtual CGameObject* Clone();
	virtual int AddPacket(BYTE* pPacket, int iOffset);
	virtual void Copy(CGameObject* Obj);



public:
	virtual bool CompareMove(const Vector3& Pos);
};

