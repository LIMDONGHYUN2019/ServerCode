#include "UserManager.h"
#include "User.h"
#include "Stream.h"
DEFINITION_SINGLE(CUserManager)

CUserManager::CUserManager() :
	m_iCapacity(0)
{
}

CUserManager::~CUserManager()
{
	End();
}

bool CUserManager::Init()
{
	m_iCapacity = 100;
	m_vecUser.resize(m_iCapacity);
	return true;
}

bool CUserManager::Begin(SOCKET hListenSocket)
{
	CSync	sync;

	if (!hListenSocket)
		return false;

	for (int i = 0; i < m_iCapacity; ++i)
	{
		CUser* pUser = new CUser;

		m_vecUser[i] = pUser;
	}

	for (int i = 0; i < m_iCapacity; ++i)
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

bool CUserManager::OtherUserSendChatting(CUser* pUser, const char* pChat)
{
	auto	iter = m_ConnectUserList.begin();
	auto	iterEnd = m_ConnectUserList.end();

	CStream	stream;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(strPacket);

	for (; iter != iterEnd; ++iter)
	{
		if (*iter == pUser)
			continue;

		int iUserNumber = 0;
		char ChracterName[USERNAME_SIZE] = {};

		//char ChatContent[MESSAGE_LENGTH] = {};
		//char	NameUTF8[MESSAGE_LENGTH] = {};
		//int	iLength = MultiByteToWideChar(CP_ACP, 0, pChat, -1, 0, 0);
		//if (iLength > 0)
		//{
		//	TCHAR	strName[MESSAGE_LENGTH] = {};
		//	MultiByteToWideChar(CP_ACP, 0, pChat, -1, strName, iLength);
		//	int iUTF8Length = WideCharToMultiByte(CP_UTF8, 0, strName, iLength, 0, 0, 0, 0);

		//	if (iUTF8Length > 0)
		//	{
		//		WideCharToMultiByte(CP_UTF8, 0, strName, iLength, NameUTF8, iUTF8Length, 0, 0);
		//	}
		//}

		//cout << pUser->GetCharacterName() << "\t:\t" << NameUTF8 << endl;

		iUserNumber = pUser->GetUserNumber();
		memcpy(ChracterName, pUser->GetCharacterName(), USERNAME_SIZE);
		//memcpy(ChatContent, pChat, MESSAGE_LENGTH);

		stream.Write<int>(&iUserNumber, sizeof(int));
		stream.Write(ChracterName, USERNAME_SIZE);
		stream.Write(pChat, MESSAGE_LENGTH);
		(*iter)->WritePacket(CP_MESSAGE, (BYTE*)strPacket, stream.GetLength());

		// 유저넘버, 캐릭터이름, 채팅내용
	}


	return true;
}
