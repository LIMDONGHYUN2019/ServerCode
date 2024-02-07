#pragma once

#include "GameInfo.h"

class CQuest
{
public:
	CQuest();
	~CQuest();

private:
	vector<_tagQuestInfo*> m_vecQuestInfo;

public:
	bool Init();
	bool SaveQuest();
	void SendQuest(class CUser* pSession);
	vector<string> csv_read_row(istream& file, char delimiter);
};

/*
* 현재 Quest를 Manager로 해서 관리를 할까 아니면 User가 Quest번호만 가지고 이 Quest클래스에서 받아오기만해서 처리를 할까 고민중.........
* 여기서는 목록만 가지고있고 내가 번호를 주면 그 번호에 맞는 Qeust세부사항을 주는 식으로.....
*/