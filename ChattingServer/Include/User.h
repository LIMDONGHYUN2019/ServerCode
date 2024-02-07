#pragma once

#include "ChattingInfo.h"
#include "Network/PacketSession.h"

class CUser :
	public CPacketSession
{
public:
	CUser();
	~CUser();

private:
	int		m_iUserNumber;
	char    m_CharacterName[USERNAME_SIZE];
	bool	m_Connect;
	 
public:
	virtual bool Begin()
	{
		if (!CPacketSession::Begin())
			return false;
		return true;
	}
	virtual bool End()
	{
		if (!CPacketSession::End())
			return false;
		
		m_iUserNumber = INT_MAX;
		memset(m_CharacterName, 0, sizeof(m_CharacterName));
		m_Connect = false;
		return true;
	}

	int GetUserNumber()	const
	{
		return m_iUserNumber;
	}

	void SetUserNumber(int iNumber)
	{
		m_iUserNumber = iNumber;
	}
	const char* GetCharacterName()
	{
		return m_CharacterName;
	}
	void SetCharacterName(char* name)	
	{
		strcpy_s(m_CharacterName, name);
		// 만약 중간에 크기를 지정하면 strlen(name) + 1 인 널값까지 넣어줘야함.... 아니면 오류
		//memcpy(m_CharacterName, name, 9);
	}
	void SetConnect(bool bConnect)
	{
		m_Connect = bConnect;
	}
	bool GetConnect()	const
	{
		return m_Connect;
	}
};

