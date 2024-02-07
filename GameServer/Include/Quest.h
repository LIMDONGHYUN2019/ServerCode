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
* ���� Quest�� Manager�� �ؼ� ������ �ұ� �ƴϸ� User�� Quest��ȣ�� ������ �� QuestŬ�������� �޾ƿ��⸸�ؼ� ó���� �ұ� �����.........
* ���⼭�� ��ϸ� �������ְ� ���� ��ȣ�� �ָ� �� ��ȣ�� �´� Qeust���λ����� �ִ� ������.....
*/