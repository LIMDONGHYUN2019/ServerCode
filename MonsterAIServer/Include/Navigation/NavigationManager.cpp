#include "NavigationManager.h"
#include "PathManager.h"
#include "Navigation.h"
#include "NavigationThread.h"
#include "ThreadManager.h"

DEFINITION_SINGLE(CNavigationManager)

CNavigationManager::CNavigationManager()
{
}

CNavigationManager::~CNavigationManager()
{
	SAFE_DELETE_MAP(m_mapNav);
}

bool CNavigationManager::Init()
{
	FILE* pFile = nullptr;

	char	FullPath[MAX_PATH] = {};

	CPathManager::GetInst()->CreatePath(FullPath, "NavList.txt", DATA_PATH);

	fopen_s(&pFile, FullPath, "rt");

	if (!pFile)
		return false;

	char	Line[256] = {};
	char*	LevelData;

	fgets(Line, 256, pFile);

	int	Count = atoi(Line);

	for (int i = 0; i < Count; ++i)
	{
		memset(Line, 0, 256);
		fgets(Line, 256, pFile);

		// , 를 기준으로 구분한다.
		char* Context = nullptr;
		const char* result = strtok_s(Line, ", ", &Context);

		char	FileName[MAX_PATH] = {};
		strcpy_s(FileName, Context);

		int	NameLength = (int)strlen(FileName);
		if (FileName[NameLength - 1] == '\n')
		{
			FileName[NameLength - 1] = 0;
		}

		if (FileName[0] == ' ')
		{
			NameLength = (int)strlen(FileName);

			for (int i = 0; i < NameLength - 1; ++i)
			{
				FileName[i] = FileName[i + 1];
			}

			FileName[NameLength - 1] = 0;
		}

		memset(FullPath, 0, MAX_PATH);
		CPathManager::GetInst()->CreatePath(FullPath, FileName, DATA_PATH);

		if (FindNavigation(result))
			continue;

		CNavigation* Nav = new CNavigation;

		Nav->Init(FullPath);

		m_mapNav.insert(make_pair(result, Nav));

		vector<class CNavigationThread*>	vecThread;

		m_mapNavThread.insert(make_pair(result, vecThread));

		// 스레드 생성
		for (int j = 0; j < 3; ++j)
		{
			CNavigation* NavClone = Nav->Clone();

			int	ThreadNumber = i * 3 + j;

			char	ThreadName[64] = {};
			sprintf_s(ThreadName, "%sThread%d", result, j);

			CThreadManager::GetInst()->InsertThread<CNavigationThread>(ThreadName);

			CNavigationThread* Thread = (CNavigationThread*)CThreadManager::GetInst()->FindThread(ThreadName);

			Thread->SetNavigationData(NavClone);

			m_mapNavThread[result].push_back(Thread);

			Thread->Start();
		}
	}

	fclose(pFile);

	// 실제 길을 찾을때는 받는 패킷
	// 내비게이션이름/몬스터번호/시작위치/도착위치/ 4가지를 받아서 결과로 길을 찾은 경로를 해당 몬스터로
	// 보내주도록 한다.

	return true;
}

bool CNavigationManager::FindPath(CPacketSession* Session, const string& NavName, int MonsterNumber, ELevel LevelType, const Vector3& StartPos, const Vector3& EndPos)
{
	CNavigationThread* Thread = FindNavigationThread(NavName);

	if (!Thread)
		return false;

	Thread->FindPath(Session, MonsterNumber, LevelType, StartPos, EndPos);

	return true;
}

CNavigation* CNavigationManager::FindNavigation(const string& Name)
{
	auto	iter = m_mapNav.find(Name);

	if (iter == m_mapNav.end())
		return nullptr;

	return iter->second;
}

CNavigationThread* CNavigationManager::FindNavigationThread(const string& Name)
{
	auto	iter = m_mapNavThread.find(Name);

	if (iter == m_mapNavThread.end())
		return nullptr;

	size_t	Size = iter->second.size();

	int		Index = 0;
	int		Count = iter->second[0]->GetWorkCount();

	// 현재 가장 일을 안하고 있는 스레드를 찾아서 반환한다.
	for (size_t i = 1; i < Size; ++i)
	{
		if (iter->second[i]->GetWorkCount() < Count)
		{
			Count = iter->second[i]->GetWorkCount();
			Index = i;
		}
	}

	return iter->second[Index];
}
