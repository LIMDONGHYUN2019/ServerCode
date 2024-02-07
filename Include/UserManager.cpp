#include "UserManager.h"
#include "UserSession.h"

DEFINITION_SINGLE(CUserManager)

CUserManager::CUserManager()
{
}

CUserManager::~CUserManager()
{
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

	return true;
}

bool CUserManager::Begin(SOCKET hListenSocket)
{
	CSync	sync;

	if (!hListenSocket)
		return false;

	m_vecUser.resize(MAX_USER);

	// ������ �������ش�.
	for (int i = 0; i < MAX_USER; ++i)
	{
		CUserSession* pUser = new CUserSession;

		m_vecUser[i] = pUser;
	}

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (!m_vecUser[i]->Begin())
		{
			CUserManager::End();
			return false;
		}

		// �񵿱� Accept ���·� ������־ ���ӵǴ¼��� �� ���ǿ� ������ �Ҵ����ش�.
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

bool CUserManager::Join(const char* pName, const char* pEmail, const char* pID, const char* pPass)
{
	// �ش� ID�� ���Ե� ������ �ִ��� Ȯ���Ѵ�.
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

	// DB�� ȸ�������� ������ �߰��Ѵ�.

	// ����� ���Ͽ� �߰��Ѵ�.
	FILE* pFile = nullptr;

	fopen_s(&pFile, "Member.mbr", "wb");

	if (pFile)
	{
		int	iCount = m_mapMember.size();

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
