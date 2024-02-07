
#include "NavigationThread.h"
#include "Navigation.h"
#include "Network/PacketSession.h"
#include "Stream.h"

CNavigationThread::CNavigationThread()
{
}

CNavigationThread::~CNavigationThread()
{
}

void CNavigationThread::Run()
{
	while (m_Loop)
	{
		// 길찾기 큐에 작업요청이 있는지를 판단한다.
		if (!m_WorkQueue.empty())
		{
			cout << m_WorkQueue.size() << endl;

			NavWorkData	Data = m_WorkQueue.front();

			m_WorkQueue.pop();

			// 가지고 있는 몬스터 번호를 이용하여 결과값을 보내준다.
			CStream	stream;

			char	Packet[MAX_BUFFER_LENGTH] = {};

			stream.SetBuffer(Packet);

			// 요청받은 정보로 길찾기를 시작한다.
			vector<Vector3>	result;
			if (m_Nav->FindPath(result, Data.StartPos, Data.EndPos))
			{
				// 패킷 : 길을 찾았는지/몬스터번호/경로 개수/경로
				bool	Find = true;
				stream.Write(&Find, sizeof(bool));
				stream.Write(&Data.MonsterNumber, sizeof(int));
				stream.Write(&Data.LevelType, sizeof(ELevel));
				
				int	PathCount = (int)result.size();
				stream.Write(&PathCount, sizeof(int));

				for (int i = 0; i < PathCount; ++i)
				{
					stream.Write(&result[i], sizeof(Vector3));
				}
			}

			else
			{
				bool	Find = false;
				stream.Write(&Find, sizeof(bool));
				stream.Write(&Data.MonsterNumber, sizeof(int));
			}

			Data.Session->WritePacket(AP_FINDPATH, (BYTE*)Packet, stream.GetLength());
		}

		Sleep(1);
	}
}

bool CNavigationThread::FindPath(CPacketSession* Session, int MonsterNumber, ELevel LevelType, const Vector3& StartPos, const Vector3& EndPos)
{
	NavWorkData	Data;

	Data.Session = Session;
	Data.MonsterNumber = MonsterNumber;
	Data.LevelType = LevelType;
	Data.StartPos = StartPos;
	Data.EndPos = EndPos;

	m_WorkQueue.push(Data);

	return true;
}
