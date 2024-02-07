#include "UserManager.h"
#include "User.h"
#include "PathManager.h"

DEFINITION_SINGLE(CUserManager)

CUserManager::CUserManager()
{
	m_iDetectUserCount = 0;
	m_iDetectUserCapacity = 8;
	m_pDetectUserArray = new CUser * [m_iDetectUserCapacity];

	m_MaxLevel = 0;
}

CUserManager::~CUserManager()
{
	SAFE_DELETE_VECLIST(m_ConnectUserList);
	SAFE_DELETE_ARRAY(m_pDetectUserArray);
}

bool CUserManager::Init()
{
	FILE* pFile = nullptr;

	fopen_s(&pFile, "Member.mbr", "rb");

	if (!pFile)
	{
		fopen_s(&pFile, "Member.mbr", "wb");

		int	iCount = 0;
		fwrite(&iCount, 4, 1, pFile);

		fclose(pFile);

		fopen_s(&pFile, "Member.mbr", "rb");
	}

	if (pFile)
	{
		int		iCount = 0;

		fread(&iCount, 4, 1, pFile);

		for (int i = 0; i < iCount; ++i)
		{
			PMemberInfo	pMember = new MemberInfo;

			memset(pMember, 0, sizeof(MemberInfo));

			fread(pMember, sizeof(MemberInfo), 1, pFile);

			hash<string>	h;
			size_t	iKey = h(pMember->strID);

			m_mapMember.insert(make_pair(iKey, pMember));
		}

		fclose(pFile);
	}

	char	FullPath[MAX_PATH] = {};

	const char* pPath = CPathManager::GetInst()->FindPathMultibyte(DATA_PATH);

	if (pPath)
		strcpy_s(FullPath, pPath);
	strcat_s(FullPath, "ExpTable.txt");

	fopen_s(&pFile, FullPath, "rt");

	if (pFile)
	{
		// 최대레벨을 읽어온다.
		char	Line[256] = {};

		fgets(Line, 256, pFile);

		m_MaxLevel = atoi(Line);

		for (int i = 0; i < m_MaxLevel; ++i)
		{
			fgets(Line, 256, pFile);

			//unsigned __int64	Exp = (unsigned __int64)atoi(Line);
			int	Exp = (int)atoi(Line);

			m_vecExpTable.push_back(Exp);
		}

		fclose(pFile);
	}

	return true;
}

bool CUserManager::Begin(SOCKET hListenSocket)
{
	CSync	sync;

	if (!hListenSocket)
		return false;

	m_vecUser.resize(MAX_USER);
	// 유저를 생성해준다.
	for (int i = 0; i < MAX_USER; ++i)
	{
		CUser* pUser = new CUser;

		m_vecUser[i] = pUser;
	}

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (!m_vecUser[i]->Begin())
		{
			CUserManager::End();
			return false;
		}

		// 비동기 Accept 상태로 만들어주어서 접속되는순간 이 세션에 소켓을 할당해준다.
		if (!m_vecUser[i]->Accept(hListenSocket))
		{
			CUserManager::End();
			return false;
		}
	}
	return true;
}

void CUserManager::End()
{
	CSync	sync;

	for (size_t i = 0; i < m_vecUser.size(); ++i)
	{
		m_vecUser[i]->End();
		SAFE_DELETE(m_vecUser[i]);
	}

	m_vecUser.clear();
}

void CUserManager::Update(float fTime)
{
	CSync	sync;

	auto	iter = m_ConnectUserList.begin();
	auto	iterEnd = m_ConnectUserList.end();

	for (; iter != iterEnd;)
	{
		if (!(*iter)->GetConnect())
		{
			iter = m_ConnectUserList.erase(iter);
			continue;
		}

		(*iter)->Update(fTime);
		++iter;
	}
}

bool CUserManager::Join(const char* pName, const char* pEmail, const char* pID, const char* pPass)
{
	// 해당 ID로 가입된 유저가 있는지 확인한다.
	PMemberInfo	pMember = FindMember(pID);

	if (pMember)
		return false;

	pMember = new MemberInfo;

	memset(pMember, 0, sizeof(MemberInfo));

	strcpy_s(pMember->strName, pName);
	strcpy_s(pMember->strEmail, pEmail);
	strcpy_s(pMember->strID, pID);
	strcpy_s(pMember->strPass, pPass);

	hash<string>	h;
	size_t	iKey = h(pID);

	m_mapMember.insert(make_pair(iKey, pMember));
	// DB에 회원가입한 정보를 추가한다.

	// 멤버를 파일에 추가한다.
	FILE* pFile = nullptr;

	fopen_s(&pFile, "Member.mbr", "wb");

	if (pFile)
	{
		int	iCount = (int)m_mapMember.size();

		fwrite(&iCount, 4, 1, pFile);

		auto	iter = m_mapMember.begin();
		auto	iterEnd = m_mapMember.end();

		for (; iter != iterEnd; ++iter)
		{
			fwrite(iter->second, sizeof(MemberInfo), 1, pFile);
		}

		fclose(pFile);
	}

	return true;
}

bool CUserManager::Login(const char* pID, const char* pPass)
{
	PMemberInfo	pMember = FindMember(pID);

	if (!pMember)
		return false;

	if (strcmp(pMember->strPass, pPass) == 0)
		return true;

	return false;
}

PMemberInfo CUserManager::FindMember(const char* pID)
{
	hash<string>	h;
	size_t	iKey = h(pID);

	auto	iter = m_mapMember.find(iKey);

	if (iter == m_mapMember.end())
		return nullptr;

	return iter->second;
}

class CUser** CUserManager::DetectUser(const Vector3& vPos, float fDetectDistance)
{
	m_iDetectUserCount = 0;

	auto	iter = m_ConnectUserList.begin();
	auto	iterEnd = m_ConnectUserList.end();

	for (; iter != iterEnd; ++iter)
	{
		float	fDist = (*iter)->GetPos().Distance(vPos);

		if (fDist <= fDetectDistance)
		{
			if (m_iDetectUserCount == m_iDetectUserCapacity)
			{
				m_iDetectUserCapacity *= 2;

				CUser** pArray = new CUser * [m_iDetectUserCapacity];

				memcpy(pArray, m_pDetectUserArray, sizeof(CUser*) * m_iDetectUserCount);

				SAFE_DELETE_ARRAY(m_pDetectUserArray);
				m_pDetectUserArray = pArray;
			}

			m_pDetectUserArray[m_iDetectUserCount] = *iter;
			++m_iDetectUserCount;
		}
	}

	// 거리별로 정렬한다.
	// 거리가 가까운 유저에서 먼 유저 순으로 정렬해준다.
	//m_DetectUserList.sort(SortDetectUser);

	return m_pDetectUserArray;
}

bool CUserManager::SortDetectUser(CUser* pSrc, CUser* pDest)
{
	return false;
}

