#pragma once

#include "GameInfo.h"
#include "ThreadSync.h"

class CUserManager :
	public CThreadSync<CUserManager>
{
private:
	vector<class CUser*>	m_vecUser;
	unordered_map<size_t, PMemberInfo>	m_mapMember;

	// ���� ���ӵ� ���� ����� �����. ������ �߰��߰� ������������ �ֱ� ������ �߰��� ������ ������ �ڷᱸ����
	// �����ؾ� �Ѵ�. �׷��Ƿ� ����Ʈ�� �̿��ؼ� �۾��ϵ��� �Ѵ�.
	list<class CUser*>	m_ConnectUserList;

	class CUser** m_pDetectUserArray;

	int			m_iDetectUserCount;
	int			m_iDetectUserCapacity;

	//vector<unsigned __int64>	m_vecExpTable;
	vector<int>	m_vecExpTable;
	int			m_MaxLevel;

public:
	//unsigned __int64 GetExp(int Level)	const
	//{
	//	return m_vecExpTable[Level - 1];                          
	//}
	int GetExp(int Level)	const
	{
		return m_vecExpTable[Level - 1];
	}

	const list<class CUser*>* GetConnectUserList()	const
	{
		return &m_ConnectUserList;
	}

	void AddConnectUser(class CUser* pUser)
	{
		m_ConnectUserList.push_back(pUser);           
	}

	void DeleteConnectUser(class CUser* pUser)
	{
		CSync	sync;
		 
		auto	iter = m_ConnectUserList.begin();
		auto	iterEnd = m_ConnectUserList.end();

		for (; iter != iterEnd; ++iter)
		{
			if (*iter == pUser)
			{
				m_ConnectUserList.erase(iter);
				cout << "GameManager User ����" << endl;
				return;
			}
		}
	}

public:
	bool Init();
	bool Begin(SOCKET hListenSocket);
	void End();
	void Update(float fTime);

public:
	bool Join(const char* pName, const char* pEmail, const char* pID, const char* pPass);
	bool Login(const char* pID, const char* pPass);
	PMemberInfo FindMember(const char* pID);


public:
	class CUser** DetectUser(const Vector3& vPos, float fDetectDistance);

private:
	static bool SortDetectUser(class CUser* pSrc, class CUser* pDest);

	DECLARE_SINGLE(CUserManager)
};

