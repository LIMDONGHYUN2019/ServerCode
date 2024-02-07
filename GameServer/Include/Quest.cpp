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
		cout << "파일이 존재하지가 않습니다. 새로 만들어주세요" << endl;

		return false;
	}

	file.seekg(sizeof(BYTE) * 3);
	/*
		엑셀에서 UTF8 csv 파일을 저장하면 UTF8-BOM(파일 앞에 3바이트가 추가 됨) 형태로 저장된다.
		사실 utf8의 경우에는 바이트 오더 마크가 필요 없지만 MS 제품에서 생성하는 utf8문서에는 자동으로 삽입된다.
		그래서 엑셀을 이용해 생성 된 csv파일은 처음 3byte가 0xEF, 0xBB, 0xBF 인 경우 bom을 나타내는 마크라고 생각하고 무시해주는 처리가 필요하다.
	*/
	while (file.good()) // 파일의 끝인 eof가 나올떄 까지
	{
		stringstream ss;
		bool inquotes = false;
		vector<string> row;

		while (file.good())
		{
			vector<string> row = csv_read_row(file, ',');

			if (!row[0].find("#")) //만약 csv 파일 안에 # 문자가 있을경우
			{
				continue;
			}
			else //#문자가 없을 경우
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

				//cout << "퀘스트 이름 : " << Quest->strName << endl;
				//cout << "퀘스트 설명 : " << Quest->strDesc << endl;

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

