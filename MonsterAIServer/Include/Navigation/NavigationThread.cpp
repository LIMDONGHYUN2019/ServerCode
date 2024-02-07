
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
		// ��ã�� ť�� �۾���û�� �ִ����� �Ǵ��Ѵ�.
		if (!m_WorkQueue.empty())
		{
			cout << m_WorkQueue.size() << endl;

			NavWorkData	Data = m_WorkQueue.front();

			m_WorkQueue.pop();

			// ������ �ִ� ���� ��ȣ�� �̿��Ͽ� ������� �����ش�.
			CStream	stream;

			char	Packet[MAX_BUFFER_LENGTH] = {};

			stream.SetBuffer(Packet);

			// ��û���� ������ ��ã�⸦ �����Ѵ�.
			vector<Vector3>	result;
			if (m_Nav->FindPath(result, Data.StartPos, Data.EndPos))
			{
				// ��Ŷ : ���� ã�Ҵ���/���͹�ȣ/��� ����/���
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
