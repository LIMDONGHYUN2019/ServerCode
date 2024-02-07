#pragma once

#include "GameInfo.h"

class CInventory
{
public:
	CInventory();
	~CInventory();

private:
	char					m_strFullPath[MAX_PATH];
	vector<_tagItemInfo*>	m_vecItem; 

public:
	bool Init(const char* pFileName);


public:
	void SendItemList(class CUser* pSession);
	bool AddItem(_tagItemInfo* pItem);
	bool PopItem(_tagItemInfo* pItem);
	bool SaveItem();
};

