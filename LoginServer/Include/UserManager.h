#pragma once

#include "LoginInfo.h"
#include "ThreadSync.h"

class CUserManager :
	public CThreadSync<CUserManager>
{
private:
	vector<class CUserSession*>	m_vecUser;
	unordered_map<size_t, PMemberInfo>	m_mapMember;

public:
	bool Init();
	bool Begin(SOCKET hListenSocket);
	void End();

public:
	bool Join(const char* pName, const char* pEmail, const char* pID, const char* pPass);
	bool Login(const char* pID, const char* pPass);
	PMemberInfo FindMember(const char* pID);

	DECLARE_SINGLE(CUserManager)
};

