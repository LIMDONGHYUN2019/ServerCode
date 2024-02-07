#include "Quest.h"
#include "Stream.h"
#include "User.h"
#include "PathManager.h"

#include <sstream>
#include <istream>
#include <fstream>

CQuest::CQuest()
{
}

CQuest::~CQuest()
{
}

bool CQuest::Init()
{
	char strPath[MAX_PATH] = {};
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);
	strcpy_s(strPath, pPath);
	strcat_s(strPath, "QuestList.csv");

	/*SetConsoleOutputCP(CP_UTF8);*/

	ifstream file(strPath);

	if (file.fail())
	{
		cout << "������ ���������� �ʽ��ϴ�. ���� ������ּ���" << endl;

		return false;
	}

	file.seekg(sizeof(BYTE) * 3);
	/*
		�������� UTF8 csv ������ �����ϸ� UTF8-BOM(���� �տ� 3����Ʈ�� �߰� ��) ���·� ����ȴ�.
		��� utf8�� ��쿡�� ����Ʈ ���� ��ũ�� �ʿ� ������ MS ��ǰ���� �����ϴ� utf8�������� �ڵ����� ���Եȴ�.
		�׷��� ������ �̿��� ���� �� csv������ ó�� 3byte�� 0xEF, 0xBB, 0xBF �� ��� bom�� ��Ÿ���� ��ũ��� �����ϰ� �������ִ� ó���� �ʿ��ϴ�.
	*/
	while (file.good()) // ������ ���� eof�� ���Ë� ����
	{
		stringstream ss;
		bool inquotes = false;
		vector<string> row;

		while (file.good())
		{
			vector<string> row = csv_read_row(file, ',');

			if (!row[0].find("#")) //���� csv ���� �ȿ� # ���ڰ� �������
			{
				continue;
			}
			else //#���ڰ� ���� ���
			{
				_tagQuestInfo* Quest = new _tagQuestInfo;
				// Number, Name, Desc, Type, Serial, Data, Exp

				Quest->QuestNumber = stoi(row[1]);

				int iSize = row[2].size();
				row[2].copy(Quest->strName, iSize);
				iSize = row[3].size();
				row[3].copy(Quest->strDesc, iSize);

				row[4].copy((char*)&Quest->Option.Type, sizeof(UINT8));

				Quest->Option.SerialNumber = stoi(row[5]);
				Quest->Option.OptionData = stoi(row[6]);
				Quest->QuestExp = stoi(row[7]);

				//cout << "����Ʈ �̸� : " << Quest->strName << endl;
				//cout << "����Ʈ ���� : " << Quest->strDesc << endl;

				m_vecQuestInfo.emplace_back(Quest);
			}
		}
	}

	file.close();

	return true;
}

vector<string> CQuest::csv_read_row(istream& file, char delimiter)
{
	stringstream ss;
	bool inquotes = false;
	vector<string> row;

	while (file.good())
	{
		char c = file.get();

		if (!inquotes && c == '"')
		{
			inquotes = true;
		}
		else if (inquotes && c == '"')
		{
			if (file.peek() == '"')
			{
				ss << (char)file.get();
			}
			else
			{
				inquotes = false;
			}
		}
		else if (!inquotes && c == delimiter)
		{
			row.push_back(ss.str());
			ss.str("");
		}
		else if (!inquotes && (c == '\r' || c == '\n'))
		{
			if (file.peek() == '\n') { file.get(); }
			row.push_back(ss.str());

			return row;
		}
		else
		{
			ss << c;
		}
	}
	
	return row;
}

bool CQuest::SaveQuest()
{
	int iQuestCount = m_vecQuestInfo.size();
	for (int i = 0; i < iQuestCount; ++i)
	{



		return true;
	}

	return false;
}

void CQuest::SendQuest(class CUser* pSession)
{
	BYTE	Packet[MAX_BUFFER_LENGTH] = {};
	CStream	stream;
	stream.SetBuffer((char*)Packet);

	int	iCount = (int)m_vecQuestInfo.size();
	stream.Write<int>(&iCount, sizeof(int));

	for (int i = 0; i < iCount; ++i)
	{
		stream.Write(&m_vecQuestInfo[i]->QuestNumber, sizeof(int));
		stream.Write(m_vecQuestInfo[i]->strName, QUESTNAME_SIZE);
		stream.Write(m_vecQuestInfo[i]->strDesc, DESC_SIZE);

		_tagQuestOption Option = m_vecQuestInfo[i]->Option;
		{
			stream.Write(&Option.Type, sizeof(EQuestType));
			stream.Write(&Option.SerialNumber, sizeof(int));
			stream.Write(&Option.OptionData, sizeof(int));
		}

		stream.Write(&m_vecQuestInfo[i]->QuestExp, sizeof(int));
	}

	pSession->WritePacket(GP_QUESTLIST, Packet, stream.GetLength());
}

