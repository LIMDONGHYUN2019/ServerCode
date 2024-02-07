#pragma once

#include "ChattingInfo.h"
#include "ThreadSync.h"
#include <iostream>

class CUserManager :
	public CThreadSync<CUserManager>
{
private:
	//class CUser** m_pUserArray;
	vector<class CUser*>	m_vecUser;
	int			m_iCapacity;

	list<class CUser*>	m_ConnectUserList;
	//vector<int>	m_vecReturnNumber;

public:
	void AddConnectUser(class CUser* pUser)
	{
		m_ConnectUserList.emplace_back(pUser);
	}
	void DisConnectUser(class CUser* pUser)
	{
		m_ConnectUserList.remove(pUser);
	}
	void ComformUserCount()
	{
		std::cout << "Current Connect UserCount : " << m_ConnectUserList.size() << std ::endl;
	}

	bool Init();
	bool Begin(SOCKET hListenSocket);
	void End();

	bool OtherUserSendChatting(class CUser* pUser, const char* pChat);
	DECLARE_SINGLE(CUserManager)
};

