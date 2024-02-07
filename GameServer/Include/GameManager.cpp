#include "GameManager.h"
#include "NetworkEngine.h"
#include "GameIocp.h"
#include "UserManager.h"
#include "DatabaseManager.h"
#include "PathManager.h"
#include "ThreadManager.h"
#include "Timer.h"
#include "MonitorThread.h"
#include "GameObject/NPC/MonsterManager.h"  
#include "GameObject/NPC/Monster.h"
#include "Network/PacketSession.h"
#include "User.h"
#include "Stream.h"
#include "Inventory.h"
#include "Quest.h"
#include "Level/LevelManager.h"
#include "Level/Level.h"
#include "Skill/SkillManager.h"
#include "Skill/SkillTree.h"

#include <fstream>
#include "include/json/json.h"
#pragma comment(lib, "jsoncpp.lib")
#pragma warning(disable: 4996)

DEFINITION_SINGLE(CGameManager)

CCriticalSection	g_MainCrt;

CGameManager::CGameManager() :
	m_pIocp(nullptr),
	m_pTimer(nullptr),
	m_bLoop(true),
	m_pAISession(nullptr)
{
}

CGameManager::~CGameManager()
{
	m_bLoop = false;

	SAFE_DELETE_VECLIST(m_vecCharInfo);
	SAFE_DELETE_VECLIST(m_vecStoreInfo);

	if (m_pIocp)
		m_pIocp->End();

	DESTROY_SINGLE(CLevelManager);

	//DESTROY_SINGLE(CGameAreaManager);

	DESTROY_SINGLE(CMonsterManager);

	DESTROY_SINGLE(CUserManager);

	DESTROY_SINGLE(CDatabaseManager);

	/*if (m_pAISession)
	{
		m_pAISession->End();
		SAFE_DELETE(m_pAISession);
	}*/

	if (m_pListen)
	{
		m_pListen->End();
		SAFE_DELETE(m_pListen);
	}

	SAFE_DELETE(m_pIocp);
	SAFE_DELETE(m_pTimer);

	DESTROY_SINGLE(CNetworkEngine);
}

bool CGameManager::Init()
{
	if (!GET_SINGLE(CNetworkEngine)->Init())
	{
		return false;
	}

	if (!GET_SINGLE(CMonsterManager)->Init())
		return false;

	if (!GET_SINGLE(CUserManager)->Init())
		return false;

	if (!GET_SINGLE(CLevelManager)->Init())
		return false;

	// Listen용 세션을 만든다.
	m_pListen = new CNetSession;

	// 서버 소켓을 바인딩 한다.
	if (!m_pListen->TcpBind())
		return false;

	if (!m_pListen->Listen(GAMESERVER_PORT, MAX_USER))
		return false;

	// Database 관리자 초기화
	if (!GET_SINGLE(CDatabaseManager)->Init("localhost", "root", "dlaehdgus3!", "Game", 3306))
		return false;

	GET_SINGLE(CSkillManager)->Init();

	// 캐릭터 정보를 읽어온다.
	FILE* pFile = nullptr;
	char	strPath[MAX_PATH] = {};

	// 경로를 가져오고
	const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);

	if (pPath)
		strcpy_s(strPath, pPath);

	strcat_s(strPath, "CharacterInfo.cif");

	fopen_s(&pFile, strPath, "rb");

	if (!pFile)
		return false;
	int	iSize = 0;
	fread(&iSize, sizeof(int), 1, pFile);
	for (int i = 0; i < iSize; ++i)
	{
		_tagCharacterInfo* pInfo = new _tagCharacterInfo;
		memset(pInfo, 0, sizeof(_tagCharacterInfo));
		fread((int*)pInfo + 1, sizeof(_tagCharacterInfo) - sizeof(int) - sizeof(int), 1, pFile);
		pInfo->iSkillPoint = 0;
		pInfo->MapLevel = ELevel::MainLevel;

		m_vecCharInfo.push_back(pInfo);
	}

	{
		ifstream ist("../Bin/Data/Store.json");
		string str;
		for (char p; ist >> p;)
			str += p;

		Json::Reader reader;
		Json::Value root;
		bool parsingRet = reader.parse(str, root);

		Json::Value ItemList = root["ItemList"];
		int iItemCount = ItemList.size();
		for (int i = 0; i < iItemCount; i++)
		{
			_tagItemInfo* pInfo = new _tagItemInfo;

			Json::Value Tempo = root[ItemList[i].asCString()];
			strcpy(pInfo->strName, Tempo[0].asCString());
			pInfo->Grade = (EItemGrade)Tempo[1].asUInt();
			pInfo->Type = (ItemType)Tempo[2].asUInt();
			pInfo->Price = Tempo[3].asInt();
			pInfo->Sell = Tempo[4].asInt();

			int iOptionCount = Tempo[5].asInt();
			for (int i = 0; i < iOptionCount; i++)
			{
				_tagItemOption pOption;
				pOption.Type = (ItemOptionType)Tempo[6 + i * 2].asInt();
				pOption.Option = Tempo[7 + i * 2].asInt();

				pInfo->vecOption.push_back(pOption);
			}

			m_vecStoreInfo.push_back(pInfo);
		}
	}
	fclose(pFile);

	// 영역관리자 초기화
	//if (!GET_SINGLE(CGameAreaManager)->Init())
	//	return false;

	// 영역을 관리할 스레드를 돌려준다.
	//if (!GET_SINGLE(CThreadManager)->InsertThread<CAreaThread>("AreaThread"))
	//	return false;

	//GET_SINGLE(CThreadManager)->FindThread("AreaThread")->Start();
	//---------------------------------------------------------------------------

	if (!GET_SINGLE(CThreadManager)->InsertThread<CMonitorThread>("MonitorThread"))
		return false;

	GET_SINGLE(CThreadManager)->FindThread("MonitorThread")->Start();
	//----------------------------------------------------------------------------

	// Login용 Iocp 를 생성한다.
	m_pIocp = new CGameIocp;

	m_pIocp->SetListenSession(m_pListen);

	m_pIocp->Begin();

	m_pTimer = new CTimer;

	m_pTimer->Init();

	// AI Server에 접속요청을 한다.
	// Listen용 세션을 만든다.
	m_pAISession = new CPacketSession;

	m_pAISession->Begin();

	if (!m_pAISession->TcpBind())
		return false;

	// 아니면 스레드를 써서 해줘야되나 

	// 서버 소켓을 바인딩 한다.
	if (!m_pAISession->Connect(AISERVER_IP, AISERVER_PORT))
		return false;

	m_pIocp->RegisterSocketToIocp(m_pAISession->GetSocket(), (ULONG_PTR)m_pAISession);

	m_pAISession->InitializeReadForIocp();

	return true;
}

void CGameManager::Run()
{
	while (m_bLoop)
	{
		// 유저 목록을 반복하며 이동중인 유저들은 이동 동기화 작업을 시작한다.
		m_pTimer->Update();

		if (m_pTimer->GetDeltaTime() >= 3.f)
			continue;

		// Queue에 데이터가 있는지 판단한다.
		// 패킷을 빠르게 못받아올거같다. 패킷한번받아오고 매니저들 업데이트 한번하고
		if (!m_PacketQueue.empty())
		{
			PacketQueueData	data = m_PacketQueue.front();

			switch (data.Protocol)
			{
			case GP_CONNECT:
				cout << data.pSession->GetSocket() << " Client Connect ●●●" << endl;
				Connect(data.Packet, data.pSession);
				break;
			case GP_DISCONNECT:
				DisConnect(data.Packet, data.pSession);
				break;
			case GP_CREATECHARACTER:
				CreateCharacter(data.Packet, data.pSession);
				break;
			case GP_SELECT_CHARACTERINFO:
				break;
			case GP_DELETE_CHARACTERINFO:
				DeleteCharacterInfo(data.Packet, data.pSession);
				break;
			case GP_CREATE_CHARACTERINFO:
				CreateCharacterInfo(data.Packet, data.pSession);
				break;
			//=========================================
			case GP_GAMESTART: // 여기서 부터 Game
				GameStart(data.Packet, data.pSession, data.Length);
				break;
			case GP_GAMEIN:
				GameIn(data.Packet, data.pSession);
				break;
			case GP_LEVELSTART:
				LevelStart(data.Packet, data.pSession);
				break;
			//=========================================
			case GP_USERAPPEAR: 
				UserAppear(data.Packet, data.pSession);
				break;
			case GP_USERDISAPPEAR:
				break;
			case GP_MOVESTART:
				MoveStart(data.Packet, data.pSession);
				break;
			case GP_MOVE:
				Move(data.Packet, data.pSession);
				break;
			case GP_MOVEEND:
				MoveEnd(data.Packet, data.pSession);
				break;
			case GP_ROTATION:
				RotationPacket(data.Packet, data.pSession);
				break;
			//=========================================
			case GP_ATTACK:
				OtherAttack(data.Packet, data.Length, data.pSession);
				break;
			//=========================================
			case GP_CHARACTERLEVELUP:
				CharacterLevelUp(data.Packet, data.pSession);
				break;
			//==========================================
			case GP_STORELIST:
				StoreList(data.Packet, data.pSession);
				break;
			//==========================================
			case GP_MONSTERITEMDROP:
				MonsterItemDrop(data.Packet, data.pSession);
				break;
			//==========================================
			case GP_SkillList:
				SkillList(data.Packet, data.pSession);
				break;
			case GP_SkillUp:
				SkillUp(data.Packet, data.pSession);
				break;
			case GP_SkillSave:
				break;
			// ========================================
			case GP_INVENTORYLIST:
				InventoryList(data.Packet, data.pSession);
				break;
			case GP_INVENTORYADD:
				InventoryAdd(data.Packet, data.pSession);
				break;
			case GP_INVENTORYDELETE:
				InventoryDelete(data.Packet, data.pSession);
				break;
			case GP_INVENTORYGOLD:
				InventoryGold(data.Packet, data.pSession);
				break;
			// ========================================
			case GP_TRADEREQUEST:
				TradeRequest(data.Packet, data.pSession);
				break;
			case GP_TRADEREQUESTACCEPT:
				TradeRequestAccept(data.Packet, data.pSession);
				break;
			case GP_TRADECANCEL:
				TradeCancel(data.Packet, data.pSession);
				break;
			case GP_TRADEADD:
				TradeAdd(data.Packet, data.pSession);
				break;
			case GP_TRADEDELETE:
				TradeDelete(data.Packet, data.pSession);
				break;
			case GP_TRADEREADY:
				TradeReady(data.Packet, data.pSession);
				break;
			case GP_TRADESUCCESS:
				TradeSuccess(data.Packet, data.pSession);
				break;
			//=========================================
			case GP_GROUPCREATE:
				GroupCreate(data.Packet, data.pSession);
				break;
			case GP_GROUPREQUEST:
				GroupRequest(data.Packet, data.pSession);
				break;
			case GP_GROUPACCEPT:
				GroupAccept(data.Packet, data.pSession);
				break;
			case GP_GROUPCANCEL:
				GroupCancel(data.Packet, data.pSession);
				break;
			case GP_GROUPLEAVE:
				GroupLeave(data.Packet, data.pSession);
				break;
			case GP_GROUPKICK:
				GroupKick(data.Packet, data.pSession);
				break;
			case GP_GROUPSUCCESS:
				GroupSuccess(data.Packet, data.pSession);
				break;
			//=========================================
			case GP_PLAYERATTACKDAMAGE:
				PlayerAttackDamage(data.Packet, data.pSession);
				break;
			case GP_MAINPORTAL:
				MainPortal(data.Packet, data.pSession);
				break;
			case GP_FINDMONSTERPATH:
				FindPath(data.Packet, (CPacketSession*)data.pSession);
				break;
			case GP_ALIVE:
				AlivePacket(data.pSession);
				break;
			}

			m_PacketQueue.pop();
		}


		float	fDeltaTime = m_pTimer->GetDeltaTime();

		GET_SINGLE(CUserManager)->Update(fDeltaTime);
		GET_SINGLE(CMonsterManager)->Update(fDeltaTime);
		GET_SINGLE(CLevelManager)->Update(fDeltaTime);
	}
}

_tagCharacterInfo* CGameManager::FindCharacterInfo(const char* job)
{
	auto	iter = m_vecCharInfo.begin();
	auto	iterEnd = m_vecCharInfo.end();

	for (int i = 0; iter != iterEnd; ++iter, ++i)
	{
		_tagCharacterInfo* info = *iter;

		if(strcmp(job, info->strJob) == 0)
			return info;
	}

	return nullptr;
}


void CGameManager::Connect(BYTE* pPacket, CUser* pSession)
{
	// 클라 서버매니저에서 보내준다.
	SelectCharacterInfo(pPacket, pSession);
}

void CGameManager::DisConnect(BYTE* pPacket, CUser* pSession)
{
}

void CGameManager::CreateCharacter(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	char	strJob[JOBNAME_SIZE] = {};
	char	strName[NAME_SIZE] = {};
	//wchar_t	strName1[NAME_SIZE] = {};

	stream.Read(strJob, JOBNAME_SIZE);
	//stream.Read(strName1, NAME_SIZE);
	stream.Read(strName, NAME_SIZE);

	//char def = '?';
	//ideCharToMultiByte(CP_ACP, 0, strName1, -1, strName, NAME_SIZE, &def, nullptr);

	std::cout << "CreateCharacter : " << strJob << std::endl;
	std::cout << "CreateCharacterName : " << strName << std::endl;

	// 이 유저가 가지고 있는 캐릭터가 몇명인지 판단하여 3명 꽉 찼을 경우에는 생성을 못하고 공간이 있다면
	// 생성하게 만들어준다.
	int	iUserNumber = pSession->GetUserNumber();
	bool	bCreate = false;

	CharacterTable	tCharTable = {};
	ESkillJob	SkillJob = ESkillJob::End;
	CharacterInfoTable TableInfo = {};
	if (GET_SINGLE(CDatabaseManager)->GetCharacterTable(tCharTable, iUserNumber))
	{
		if (tCharTable.iCharacterCount < 3)
		{
			++tCharTable.iCharacterCount;

			bCreate = true;

			_tagCharacterInfo* info = GET_SINGLE(CGameManager)->FindCharacterInfo(strJob);
			TableInfo.iCharacterTableNumber = tCharTable.iTableNumber;
			TableInfo.iUserNumber = iUserNumber;
			TableInfo.iMapLevel = (int)info->MapLevel;
			memcpy(TableInfo.strJob, info->strJob, JOBNAME_SIZE);
			memcpy(TableInfo.strName, strName, NAME_SIZE);
			TableInfo.iAttack = info->iAttack;
			TableInfo.fAttackSpeed = info->fAttackSpeed;
			TableInfo.iArmor = info->iArmor;
			TableInfo.fMoveSpeed = info->fMoveSpeed;
			TableInfo.fCritical = info->fCritical;
			TableInfo.fCriticalDamage = info->fCriticalDamage;
			TableInfo.iHP = info->iHP;
			TableInfo.iHPMax = info->iHPMax;
			TableInfo.iMP = info->iMP;
			TableInfo.iMPMax = info->iMPMax;
			TableInfo.iLevel = 1;
			TableInfo.iExp = 0;
			TableInfo.iMoney = 1000;
			TableInfo.iSkillPoint = 2;

			GET_SINGLE(CDatabaseManager)->SaveCharacterInfo(TableInfo, iUserNumber, tCharTable.iCharacterCount);

			GET_SINGLE(CDatabaseManager)->CreateCharacterTransform(iUserNumber, TableInfo.iCharacterNumber);

			if (strcmp(TableInfo.strJob, "GreyStone") == 0)
			{
				SkillJob = ESkillJob::GrayStone;
			}
			if (strcmp(TableInfo.strJob, "TwinBlast") == 0)
			{
				SkillJob = ESkillJob::TwinBlast;
			}
			if (strcmp(TableInfo.strJob, "Serath") == 0)
			{
				SkillJob = ESkillJob::Serath;
			}

			CSkillTree* Tree = CSkillManager::GetInst()->GetSkillTree(SkillJob);

			if (Tree)
				Tree = Tree->Clone();

			pSession->SetSkillTree(Tree);

			Tree->Save();

			// 캐릭터 선택시 직업별 스킬트리 가져오고 해당 캐릭터의 스킬 정보를 얻어와서 배운 스킬가 안배운 스킬을 구분한다.
			// 정보는 파일로 만들고 파일로부터 얻어오도록 한다.
			// 캐릭터생성시 1렙 스킬을 바로 배워놓던가 아니면 디아블로처럼 스킬 포인터를 만들어서 재구성하던가

			// DB에 생성된 캐릭터 정보를 넣어준다. 이건 숙제니까 직접 만들어보자.

			// 이 캐릭터와 연결된 인벤토리를 만들어낸다.
			FILE* pFile = nullptr;

			char	strPath[MAX_PATH] = {};
			const char* pPath = GET_SINGLE(CPathManager)->FindPathMultibyte(DATA_PATH);

			if (pPath)
				strcpy_s(strPath, pPath);

			char	FileName[MAX_PATH] = {};
			sprintf_s(FileName, "Inventory_%d_%d.inv", iUserNumber, TableInfo.iCharacterNumber);

			strcat_s(strPath, FileName);

			fopen_s(&pFile, strPath, "wb");

			_tagItemInfo Potion[2]; 
			int	iInvenCount = 0;

			for (int i = 0; i < m_vecStoreInfo.size(); ++i)
			{
				if (m_vecStoreInfo[i]->Type == ItemType::IT_POTION)
				{
					Potion[iInvenCount] = *m_vecStoreInfo[i];

					iInvenCount++;

				}
			}

			if (pFile)
			{
				fwrite(&iInvenCount, 4, 1, pFile);

				for (int i = 0; i < iInvenCount; ++i)
				{
					fwrite(Potion[i].strName, 1, 32, pFile);
					fwrite(&Potion[i].Grade, sizeof(EItemGrade), 1, pFile);
					fwrite(&Potion[i].Type, sizeof(ItemType), 1, pFile);
					fwrite(&Potion[i].Price, sizeof(int), 1, pFile);
					fwrite(&Potion[i].Sell, sizeof(int), 1, pFile);

					int	iOptionCount = (int)Potion[i].vecOption.size();

					fwrite(&iOptionCount, 4, 1, pFile);
					fwrite(&Potion[i].vecOption[0], sizeof(_tagItemOption), iOptionCount, pFile);
				}

				fclose(pFile);
			}
			
			
		}
	}

	CStream		stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);
	stream1.Write(&bCreate, 1);
	//stream1.Write(&tCharTable.iCharacterCount, sizeof(int));
	stream1.Write(&SkillJob, sizeof(int));
	stream1.Write(&TableInfo, sizeof(CharacterInfoTable));

	int iLength = stream1.GetLength();
	pSession->WritePacket(GP_CREATECHARACTER, (BYTE*)strPacket, iLength);

	//CLevel* Level = CLevelManager::GetInst()->FindLevel((ELevel)_tagCharacterInfo.iMapLevel);
	//pSession->SetLevel(Level);
	////Level->AddUser(pSession);
	//Level->SendMonsterInfo(pSession);

	//pSession->GetLevel()->AddUser(pSession);
}

void CGameManager::SelectCharacterInfo(BYTE* pPacket, class CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	int	iUserNumber = -1;
	int	iLength = 0;
	TCHAR	_strID[256] = {};
	stream.Read<int>(&iUserNumber, sizeof(int));
	stream.Read<int>(&iLength, sizeof(int));
	stream.Read(_strID, iLength * sizeof(TCHAR));

	pSession->SetUserNumber(iUserNumber);
	char	strID[256] = {};
	int iCount = WideCharToMultiByte(CP_ACP, 0, _strID, -1, 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, _strID, -1, strID, iCount, 0, 0);

	cout << "Game Connect  -  ";
	cout << "User Number : " << iUserNumber << "  and  ID : " << strID << endl;

	CStream		stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);
	bool	bFail = false;

	// 보낼 패킷
	// 생성된캐릭터수 / 캐릭터1 / 캐릭터2 / 캐릭터3
	// 캐릭터 정보는 생성된 수만큼만 채운다.

	// CharacterTable 데이터베이스 테이블 정보를 유저번호를 이용해서 얻어온다.
	CharacterTable	tCharTable = {};
	if (GET_SINGLE(CDatabaseManager)->GetCharacterTable(tCharTable, iUserNumber))
	{
		// 생성된 캐릭터 수를 넣어준다.
		stream1.Write(&tCharTable.iCharacterCount, 4);

		for (int i = 0; i < ID_CharacterMax; ++i)
		{
			if (tCharTable.iCharacterNumber[i] == 0)
				continue;

			CharacterInfoTable	tCharInfoTable = {};
			if (GET_SINGLE(CDatabaseManager)->GetCharacterInfoTable(tCharInfoTable, tCharTable.iCharacterNumber[i]))
			{
				int	iJob = -1;

				if (strcmp(tCharInfoTable.strJob, "GreyStone") == 0)
				{
					iJob = 0;
				}

				else if (strcmp(tCharInfoTable.strJob, "TwinBlast") == 0)
				{
					iJob = 1;
				}

				else if (strcmp(tCharInfoTable.strJob, "Serath") == 0)
				{
					iJob = 2;
				}
				int iSelectPos = i;
				stream1.Write(&iSelectPos, sizeof(int));
				stream1.Write(&iJob, 4);
				stream1.Write(&tCharInfoTable, sizeof(CharacterInfoTable));
			}

			else
			{
				bFail = true;
				break;
			}
		}
	}

	else
		bFail = true;

	iLength = stream1.GetLength();

	if (bFail)
	{
		memset(strPacket, 0, MAX_BUFFER_LENGTH);
		memcpy(strPacket, &bFail, 1);
		iLength = 1;
		pSession->WritePacket(GP_CONNECT_FAIL, (BYTE*)strPacket, iLength);
	}

	else
		pSession->WritePacket(GP_CONNECT, (BYTE*)strPacket, iLength);
}

void CGameManager::CreateCharacterInfo(BYTE* pPacket, CUser* pSession)
{
	CStream		stream1;
	char	strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	const vector<_tagCharacterInfo*>* pvecCharInfo = GET_SINGLE(CGameManager)->GetCharInfo();

	int iSize = (int)pvecCharInfo->size();

	stream1.Write<int>(&iSize, sizeof(int));

	auto	iter = pvecCharInfo->begin();
	auto	iterEnd = pvecCharInfo->end();

	for (int i = 0; iter != iterEnd; ++iter, ++i)
	{
		stream1.Write<_tagCharacterInfo>(*iter, sizeof(_tagCharacterInfo));
	}

	int iLength = stream1.GetLength();

	pSession->WritePacket(GP_CREATE_CHARACTERINFO, (BYTE*)strPacket, iLength);
}

void CGameManager::DeleteCharacterInfo(BYTE* pPacket, CUser* pSession)
{
	CStream stream;

	stream.SetBuffer((char*)pPacket);
	int	iCharacterNumber = -1;
	stream.Read<int>(&iCharacterNumber, sizeof(int));
	pSession->SetCharacterNumber(iCharacterNumber);
	int UserNumber = pSession->GetUserNumber();
	if (GET_SINGLE(CDatabaseManager)->DeleteCharacter(iCharacterNumber, UserNumber))
	{
		cout << "CharacterNumber : " << iCharacterNumber << "Delete Complete..!!" << endl;
		CStream		stream1;
		char	strPacket[MAX_BUFFER_LENGTH] = {};
		stream1.SetBuffer(strPacket);
		stream1.Write(&iCharacterNumber, sizeof(iCharacterNumber));

		DWORD iLength = stream1.GetLength();

		pSession->WritePacket(GP_DELETE_CHARACTERINFO, (BYTE*)strPacket, iLength);
	}
}

void CGameManager::GameStart(BYTE* pPacket, CUser* pSession, int iLength)
{
	// 캐릭터 선택창에서 원하는 캐릭터를 선택하고 게임시작 버튼을 눌렀을때 들어온다.
	CStream	 stream;

	stream.SetBuffer((char*)pPacket);

	int	iCharacterNumber = -1;
	stream.Read<int>(&iCharacterNumber, sizeof(int));

	int	iJobLength = 0;
	stream.Read<int>(&iJobLength, sizeof(int));

	char	strJobName[JOBNAME_SIZE] = {};
	stream.Read(strJobName, iJobLength);

	pSession->SetCharacterNumber(iCharacterNumber);
	pSession->SetJobName(strJobName);

	ESkillJob	SkillJob = ESkillJob::End;

	if (strcmp(strJobName, "GreyStone") == 0)
	{
		SkillJob = ESkillJob::GrayStone;
	}

	else if (strcmp(strJobName, "TwinBlast") == 0)
	{
		SkillJob = ESkillJob::TwinBlast;
	}

	else if (strcmp(strJobName, "Serath") == 0)
	{
		SkillJob = ESkillJob::Serath;
	}

	// Character 정보를 GP_GAMEIN 패킷으로 만들어서 하나 보내준다.
	CharacterInfoTable	tCharTable = {};
	if (GET_SINGLE(CDatabaseManager)->GetCharacterInfoTable(tCharTable, iCharacterNumber))
	{
		// 여기에 어떤 레벨에서 시작해야 하는지 정보를 보내준다.
		CStream stream2;

		BYTE	StartPacket[MAX_BUFFER_LENGTH] = {};

		stream2.SetBuffer((char*)StartPacket);

		stream2.Write<int>(&tCharTable.iMapLevel, sizeof(int));

		pSession->WritePacket(GP_GAMESTART, StartPacket, stream2.GetLength()); // 레벨 전환용

		Vector3	vPos;
		Vector3 vRot;
		Vector3 vScale;
		
		GET_SINGLE(CDatabaseManager)->GetPlayerPos(iCharacterNumber, vPos.x, vPos.y, vPos.z, vScale.x, vScale.y, vScale.z, vRot.x, vRot.y, vRot.z);

		pSession->SetPos(vPos.x, vPos.y, vPos.z);
		pSession->SetScale(vScale.x, vScale.y, vScale.z);
		pSession->SetRotation(vRot.x, vRot.y, vRot.z);
		pSession->SetPrevPos(vPos);
		//--------------------------------------------------------------------------------------
		// 인벤토리를 생성한다.
		char	FileName[MAX_PATH] = {};
		sprintf_s(FileName, "Inventory_%d_%d.inv", pSession->GetUserNumber(), iCharacterNumber);

		CInventory* pInventory = new CInventory;

		if (!pInventory->Init(FileName))
			return;

		pSession->SetInventory(pInventory);
		pSession->SendInventory();

		class CQuest* pQuest = new CQuest;
		if (!pQuest->Init())
			return;
		pSession->SetQuest(pQuest);
		pSession->SendQuest();
		//--------------------------------------------------------------------------------------
		CSkillTree* Tree = CSkillManager::GetInst()->GetSkillTree(SkillJob);

		if (Tree)
			Tree = Tree->Clone();

		pSession->SetSkillTree(Tree);

		Tree->Load();
		//--------------------------------------------------------------------------------------
		CStream	stream1;

		char	strPacket[MAX_BUFFER_LENGTH] = {};
		stream1.SetBuffer(strPacket);

		iCharacterNumber = tCharTable.iCharacterNumber;
		stream1.Write(&iCharacterNumber, sizeof(int));
		stream1.Write(&vPos, sizeof(Vector3));
		stream1.Write(&vRot, sizeof(Vector3));
		stream1.Write(&vScale, sizeof(Vector3));

		// 현재 레벨에 획득하여 하는 경험치를 얻어온다.
		tCharTable.iExpMax = CUserManager::GetInst()->GetExp(tCharTable.iLevel);

		_tagCharacterInfo tCharaterInfo;
		memcpy(&tCharaterInfo, (int*)&tCharTable + 3, sizeof(tCharaterInfo));

		pSession->SetChracterInfo(&tCharaterInfo);
		pSession->SetUserName(tCharaterInfo.strName);
		pSession->SetDeath(false);

		stream1.Write<_tagCharacterInfo>(&tCharaterInfo, sizeof(_tagCharacterInfo));
		unsigned int length = stream1.GetLength();
		pSession->WritePacket(GP_GAMEIN, (BYTE*)strPacket, length);
		std::cout << "Send Character Info GameIn" << std::endl;

		CLevel* Level = CLevelManager::GetInst()->FindLevel((ELevel)tCharTable.iMapLevel);
		pSession->SetLevel(Level);
		//Level->AddUser(pSession);
		Level->SendMonsterInfo(pSession);
	}
}

void CGameManager::GameIn(BYTE* pPacket, CUser* pSession)
{
}

void CGameManager::LevelStart(BYTE* pPacket, CUser* pSession)
{
	//pSession->GetLevel()->SendMonsterInfo(pSession);
	pSession->GetLevel()->AddUser(pSession);
}

void CGameManager::CharacterLevelUp(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	_tagCharacterInfo Info;
	stream.Read(&Info, sizeof(_tagCharacterInfo));

	pSession->SetChracterInfo(&Info);

	pSession->UpdateCharacterInfo();
}

void CGameManager::MonsterItemDrop(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);
}

void CGameManager::UserAppear(BYTE* pPacket, class CUser* pSession)
{
	// 위치정보를 받아서 유저에 저장한다.
	CStream	stream;

	stream.SetBuffer((char*)pPacket);

	Vector3	vPos;
	Vector3 vScale;
	Vector3 vRot;

	stream.Read<Vector3>(&vPos, sizeof(Vector3));
	pSession->SetPos(vPos);
	stream.Read<Vector3>(&vScale, sizeof(Vector3));
	pSession->SetScale(vScale);
	stream.Read<Vector3>(&vRot, sizeof(Vector3));
	pSession->SetRotation(vRot);

	// 패킷에서 레벨정보를 읽어온다. 새로 접속한 유저가 어떤 레벨에 있는지를 판단해야 한다.
	std::cout << pSession->GetSocket() << " Appear Client Pos [" << vPos.x << "," << vPos.y << ","
		<< vPos.z << "]" << std::endl;

	//pSession->GetLevel()->AddUser(pSession);
	pSession->GetLevel()->AddAreaUser(pSession);

}

void CGameManager::MoveStart(BYTE* pPacket, CUser* pSession)
{
	// 이동 시작시 패킷 구조
	// 이동 시작 위치(Vector3), 이동 방향(Vector3)
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	Vector3	vPos, vDir, vRot;

	stream.Read<Vector3>(&vPos, sizeof(Vector3));
	stream.Read<Vector3>(&vRot, sizeof(Vector3));
	stream.Read<Vector3>(&vDir, sizeof(Vector3));

	float	ViewDir = 0.f;
	stream.Read<float>(&ViewDir, sizeof(float));

	//std::cout << pSession->GetSocket() << "- 시작 [" << vPos.x << "," << vPos.y << "," << vPos.z << "]"
	//	<< " / 방향 [" << vDir.x << "," << vDir.y << "," << vDir.z << "]" << std::endl;

	// 이동 시작이 들어왔으면 이제 서버에서도 이 유저의 위치를 실시간으로 이 방향으로 이동시켜준다.
	// 만약 다음 위치가 터무니없이 어긋난다면 서버의 위치로 갱신하도록 클라이언트에 보내주어야 한다.
	// 현재 유저에게 이동 위치와 방향을 저장해준다. 그러면서 이동중이라는 표시를 해주도록 한다.
	//pSession->SetPos(vPos);
	pSession->SetMoveDir(vDir);
	pSession->SetMove(true);
	pSession->SetViewDir(ViewDir);
	pSession->SetRotation(vRot);
	pSession->SetPrevPos(vPos);

	// 클라이언트가 보낸 위치와 현재 서버에 저장되어 있는 위치를 비교하여 위치가 너무 차이가 날 경우
	// 위치를 보정하고 이동을 시작할 수 있도록 한다.
	float fLength = vPos.Distance(pSession->GetPos());
	if (fLength > 100.0)
	{
		CStream	stream2;
		char	strPacket[MAX_BUFFER_LENGTH] = {};
		stream2.SetBuffer(strPacket);

		Vector3 vServerPos = pSession->GetPos();
		Vector3 vServerRot = pSession->GetRotation();

		stream2.Write<Vector3>(&vServerPos, sizeof(Vector3));
		stream2.Write<Vector3>(&vServerRot, sizeof(Vector3));

		pSession->WritePacket(GP_SYNC, (BYTE*)strPacket, stream2.GetLength());
	}

	// 패킷을 보낸 클라이언트에 현재 서버의 위치 정보를 넘겨주어 잘못된 위치인지를 체크한다.

	pSession->SetPos(vPos);


	// 주변 유저들에게 이 유저의 위치정보를 넘겨준다.	
	pSession->GetLevel()->SendMoveStartPacket(pSession);
}

void CGameManager::Move(BYTE* pPacket, CUser* pSession)
{
	// 이동 패킷 구조
	// 현재 위치(Vector3), 이동 방향(Vector3)
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	Vector3	vPos, vDir, vRot;
	float	ViewDir = 0.f;

	stream.Read<Vector3>(&vPos, sizeof(Vector3));
	stream.Read<Vector3>(&vRot, sizeof(Vector3));
	stream.Read<Vector3>(&vDir, sizeof(Vector3));
	stream.Read<float>(&ViewDir, sizeof(float));

	pSession->SetViewDir(ViewDir);
	pSession->SetRotation(vRot);
	pSession->SetMoveDir(vDir);

	// 클라이언트의 위치를 넘겨주어 위치정보를 비교한다.
	// 만약 잘못되었다면 클라이언트에 실제 정보를 보내놓게 된다.
	pSession->CompareMove(vPos);

	//// 서버위치와 클라이언트 위치의 비교처리를 한다.
	//float Length = vPos.Distance(vServerPos);

	//// 클라이언트와 너무 터무니 없이 거리차이가 날 경우 클라이언트가 핵을 쓰고 있을 가능성이 높다는 것이다.
	//// 그러므로 이러한 경우라면 서버의 위치를 기준으로 클라이언트와 동기화를 하고 그것이 아니라면 클라이언트의
	//// 위치를 기반으로 서버의 위치를 동기화 해주도록 한다.
	//if (Length > 3000.f)
	//{
	//	// 클라이언트에게 위치가 잘못되었기 때문에 보내준 위치로 강제로 맞추도록 패킷을 보낸다.
	//	// GP_CYNC를 이 패킷의 헤더로 사용한다.
	//	CStream	stream1;
	//	char	strPacket[MAX_BUFFER_LENGTH] = {};
	//	stream1.SetBuffer(strPacket);

	//	// 위치정보
	//	stream1.Write<Vector3>(&vServerPos, sizeof(Vector3));

	//	stream1.Write<Vector3>(&vServerRot, sizeof(Vector3));

	//	pSession->WritePacket(GP_SYNC, (BYTE*)strPacket, stream1.GetLength());
	//}

	//else
	//{
	//	// 클라이언트의 위치를 기준으로 서버 위치를 동기화한다.
	//	//pSession->SetPos(vPos);
	//}

	//vServerPos = pSession->GetPos();

	//std::cout << "방향전환 위치(Client) : " << vPos.x << ", " << vPos.y << ", " << vPos.z << std::endl;
	//std::cout << "방향전환 위치(Server) : " << vServerPos.x << ", " << vServerPos.y << ", " << vServerPos.z << std::endl;

	// 주변 유저들에게 이 유저의 위치정보를 넘겨준다.
	pSession->GetLevel()->SendMovePacket(pSession);
}

void CGameManager::MoveEnd(BYTE* pPacket, CUser* pSession)
{
	// 이동 패킷 구조
	// 현재 위치(Vector3)
	std::cout << "이동 종료" << std::endl;
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	Vector3	vPos, vDir, vRot;

	stream.Read<Vector3>(&vPos, sizeof(Vector3));
	stream.Read<Vector3>(&vRot, sizeof(Vector3));
	//pSession->SetPos(vPos);

	// 이동을 멈출때 이동중이 아니라고 판단한다.
	pSession->SetMove(false);
	pSession->SetViewDir(0.f);
	pSession->SetRotation(vRot);
	pSession->CompareMove(vPos);

	//// 서버위치와 클라이언트 위치의 비교처리를 한다.
	//float Length = vPos.Distance(vServerPos);

	//// 클라이언트와 너무 터무니 없이 거리차이가 날 경우 클라이언트가 핵을 쓰고 있을 가능성이 높다는 것이다.
	//// 그러므로 이러한 경우라면 서버의 위치를 기준으로 클라이언트와 동기화를 하고 그것이 아니라면 클라이언트의
	//// 위치를 기반으로 서버의 위치를 동기화 해주도록 한다.
	//if (Length > 3000.f)
	//{
	//	// 클라이언트에게 위치가 잘못되었기 때문에 보내준 위치로 강제로 맞추도록 패킷을 보낸다.
	//	// GP_CYNC를 이 패킷의 헤더로 사용한다.
	//	CStream	stream1;
	//	char	strPacket[MAX_BUFFER_LENGTH] = {};
	//	stream1.SetBuffer(strPacket);

	//	// 위치정보
	//	stream1.Write<Vector3>(&vServerPos, sizeof(Vector3));

	//	stream1.Write<Vector3>(&vServerRot, sizeof(Vector3));

	//	pSession->WritePacket(GP_SYNC, (BYTE*)strPacket, stream1.GetLength());
	//}

	//else
	//{
	//	// 클라이언트의 위치를 기준으로 서버 위치를 동기화한다.


	//	pSession->SetPos(vPos);
	//}

	//vServerPos = pSession->GetPos();

	//std::cout << "종료 위치(Client) : " << vPos.x << ", " << vPos.y << ", " << vPos.z << std::endl;
	//std::cout << "종료 위치(Server) : " << vServerPos.x << ", " << vServerPos.y << ", " << vServerPos.z << std::endl;


	// 주변 유저들에게 이 유저의 위치정보를 넘겨준다.
	pSession->GetLevel()->SendMoveEndPacket(pSession);
}

void CGameManager::RotationPacket(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	float	Yaw = 0.f;

	stream.Read<float>(&Yaw, sizeof(float));

	pSession->GetLevel()->OtherRotation(pSession, Yaw);
}

void CGameManager::OtherAttack(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);
	AttackType eAttack_Type = AttackType::None;
	stream.Read<AttackType>(&eAttack_Type, sizeof(bool));
	if (eAttack_Type != AttackType::None)
	{
		// 스킬 종류가 너무 다양해 스킬에 부가적인 설정이 붙을수 있어서 
		// 일반 공격이랑 통합하기에 약간 고민.
		pSession->GetLevel()->OtherAttack(pSession, eAttack_Type);
	}
}

void CGameManager::OtherAttack(BYTE* pPacket, int Length, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);
	AttackType eAttack_Type = AttackType::None;
	stream.Read<AttackType>(&eAttack_Type, sizeof(bool));

	int AttackIndex = -1;
	stream.Read(&AttackIndex, sizeof(int));

	float	ViewDir = 0.f;
	stream.Read<float>(&ViewDir, sizeof(float));
	pSession->SetViewDir(ViewDir);

	if (eAttack_Type == AttackType::NormalAttack)
	{
		// 스킬 종류가 너무 다양해 스킬에 부가적인 설정이 붙을수 있어서 
		// 일반 공격이랑 통합하기에 약간 고민.
		pSession->GetLevel()->OtherAttack(pSession, AttackIndex);
	}
}

void CGameManager::SkillList(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	pSession->SendSkill();
}

void CGameManager::SkillUp(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	int	iSkillNumber = 0;
	stream.Read(&iSkillNumber, sizeof(int));

	//--------------------------------------

	CStream stream2;
	char	Packet[MAX_BUFFER_LENGTH] = {};
	stream2.SetBuffer(Packet);

	int Index = -1;
	int Level = -1;

	ESkillUpMessage Msg = pSession->GetSkillTree()->SkillUp(iSkillNumber, Index, Level);
	int	ConvertMsg = (int)Msg;
	stream2.Write(&ConvertMsg, sizeof(int));
	if (Msg == ESkillUpMessage::Success)
	{
		int Point = pSession->GetChracterInfo().iSkillPoint;
		stream2.Write(&iSkillNumber, sizeof(int));
		stream2.Write(&Point, sizeof(int));
		stream2.Write(&Index, sizeof(int));
		stream2.Write(&Level, sizeof(int));
	}

	pSession->WritePacket(GP_SkillUp, (BYTE*)Packet, stream2.GetLength());
}

void CGameManager::SkillSave(BYTE* pPacket, CUser* pSession)
{
	// 스킬의 변경 내용을 저장을 해야된다.

}

void CGameManager::StoreList(BYTE* pPacket, CUser* pSession)
{
	BYTE	packet[MAX_BUFFER_LENGTH] = {};
	CStream	stream;
	stream.SetBuffer((char*)packet);

	int	iCount = (int)m_vecStoreInfo.size();
	stream.Write<int>(&iCount, sizeof(int));

	auto	iter1 = m_vecStoreInfo.begin();
	auto	iter1End = m_vecStoreInfo.end();

	for (; iter1 != iter1End; ++iter1)
	{
		stream.Write((*iter1)->strName, ITEMNAME_SIZE);
		stream.Write<EItemGrade>(&(*iter1)->Grade, sizeof(BYTE));
		stream.Write<ItemType>(&(*iter1)->Type, sizeof(BYTE));
		stream.Write<int>(&(*iter1)->Price, sizeof(int));
		stream.Write<int>(&(*iter1)->Sell, sizeof(int));

		int iOptionCount = (int)(*iter1)->vecOption.size();
		stream.Write<int>(&iOptionCount, sizeof(int));
		for (int i = 0; i < iOptionCount; i++)
		{
			stream.Write<ItemOptionType>(&(*iter1)->vecOption[i].Type, sizeof(ItemOptionType));
			stream.Write<int>(&(*iter1)->vecOption[i].Option, sizeof(int));
		}
	}
	pSession->WritePacket(GP_STORELIST, packet, stream.GetLength());
}

void CGameManager::InventoryList(BYTE* pPacket, CUser* pSession)
{
	CStream	stream;
	stream.SetBuffer((char*)pPacket);

	pSession->SendInventory();
}

void CGameManager::InventoryAdd(BYTE* pPacket, CUser* pSession)
{
	// 인벤토리에 추가해준다.
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	_tagItemInfo* pItem = new _tagItemInfo;

	stream.Read(pItem->strName, ITEMNAME_SIZE);
	stream.Read(&pItem->Grade, sizeof(EItemGrade));
	stream.Read(&pItem->Type, sizeof(ItemType));
	stream.Read(&pItem->Price, sizeof(int));
	stream.Read(&pItem->Sell, sizeof(int));

	int iOptionCount = 0;
	stream.Read(&iOptionCount, sizeof(int));
	for (int i = 0; i < iOptionCount; i++)
	{
		_tagItemOption tOption;
		stream.Read(&tOption, sizeof(tOption));

		pItem->vecOption.push_back(tOption);
	}

	bool bSuccess = pSession->AddInventory(pItem);

	char Packet[MAX_BUFFER_LENGTH] = {};
	stream.SetBuffer(Packet);
	if (bSuccess)
	{
		stream.Write(&bSuccess, sizeof(bool));

		stream.Write(&pItem->strName, ITEMNAME_SIZE);
		stream.Write(&pItem->Grade, sizeof(EItemGrade));
		stream.Write(&pItem->Type, sizeof(ItemType));
		stream.Write(&pItem->Price, sizeof(int));
		stream.Write(&pItem->Sell, sizeof(int));
		int iOptionCount = (int)pItem->vecOption.size();
		stream.Write(&iOptionCount, sizeof(int));

		for (int i = 0; i < pItem->vecOption.size(); i++)
		{
			stream.Write(&pItem->vecOption[i], sizeof(pItem->vecOption[i]));
		}
	}
	else
	{
		stream.Write(&bSuccess, sizeof(bool));
	}

	pSession->WritePacket(GP_INVENTORYADD, (BYTE*)Packet, stream.GetLength());
}

void CGameManager::InventoryDelete(BYTE* pPacket, CUser* pSession)
{
	// 인벤토리에서 제거할떄 - 상점에 팔떄 or 땅에다 버릴떄


}

void CGameManager::InventoryGold(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iCharacterNumber = 0;
	int iGold = 0;

	stream.Read(&iCharacterNumber, sizeof(int));
	stream.Read(&iGold, sizeof(int));

	
}

void CGameManager::TradeRequest(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iOtherPlayerCharacterNumber = 0;
	char PlayerCharacterName[NAME_SIZE] = {};
	int RequestCharacterNumber = 0;

	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto	iter = pUserList->begin();
	auto	iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			CStream		stream1;
			char	strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);

			memcpy(PlayerCharacterName, pSession->GetChracterInfo().strName, NAME_SIZE);
			RequestCharacterNumber = pSession->GetCharacterNumber();
			//memset(pPacket, 0, MAX_BUFFER_LENGTH);
			//stream.SetBuffer((char*)pPacket);		

			stream1.Write(&RequestCharacterNumber, sizeof(int));
			stream1.Write(PlayerCharacterName, NAME_SIZE);

			(*iter)->WritePacket(GP_TRADEREQUEST, (BYTE*)strPacket, stream1.GetLength());
		}
	}
}

void CGameManager::TradeRequestAccept(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int RequestCharacterNumber = 0;
	stream.Read(&RequestCharacterNumber, sizeof(int));

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == RequestCharacterNumber)
		{
			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);

			(*iter)->WritePacket(GP_TRADEREQUESTACCEPT, (BYTE*)strPacket, stream1.GetLength());
		}
	}
}

void CGameManager::TradeCancel(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iOtherPlayerCharacterNumber = 0;
	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();

	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);


			(*iter)->WritePacket(GP_TRADECANCEL, (BYTE*)strPacket, stream1.GetLength());
		}
	}
}

void CGameManager::TradeAdd(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);
	_tagItemInfo Item = {};

	int iOtherPlayerCharacterNumber = 0;
	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	char ItemName[ITEMNAME_SIZE] = {};
	stream.Read(ItemName, ITEMNAME_SIZE);

	BYTE Tempo;

	EItemGrade Grade;
	stream.Read(&Grade, sizeof(BYTE));
	//Grade = (EItemGrade)Tempo;

	ItemType eType = ItemType::IT_END; //크기는 4바이트인데 메모리복사하는건 1바이트니 값오류
	stream.Read(&Tempo, sizeof(BYTE));
	eType = (ItemType)Tempo;

	int iPrice;
	stream.Read(&iPrice, sizeof(int));
	int iSell;
	stream.Read(&iSell, sizeof(int));

	int iOptionCount = 0;
	stream.Read(&iOptionCount, sizeof(iOptionCount));
	for (int i = 0; i < iOptionCount; i++)
	{
		_tagItemOption tOption;
		stream.Read(&tOption, sizeof(tOption));

		Item.vecOption.push_back(tOption);
	}

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);

			stream1.Write(ItemName, ITEMNAME_SIZE);
			stream1.Write(&Grade, sizeof(BYTE));
			stream1.Write(&eType, sizeof(BYTE));
			stream1.Write(&iPrice, sizeof(int));
			stream1.Write(&iSell, sizeof(int));

			stream1.Write(&iOptionCount, sizeof(int));
			for (int i = 0; i < iOptionCount; i++)
			{
				stream1.Write(&Item.vecOption[i], sizeof(_tagItemOption));
			}


			(*iter)->WritePacket(GP_TRADEADD, (BYTE*)strPacket, stream1.GetLength());

			break;
		}
	}
}

void CGameManager::TradeDelete(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iOtherPlayerCharacterNumber = 0;
	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	int iIndex = -1;
	stream.Read(&iIndex, sizeof(int));

	// ------------------------------------------------------

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);

			stream1.Write(&iIndex, sizeof(int));


			(*iter)->WritePacket(GP_TRADEDELETE, (BYTE*)strPacket, stream1.GetLength());

			break;
		}
	}
}

void CGameManager::TradeReady(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iOtherPlayerCharacterNumber = 0;
	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);

			(*iter)->WritePacket(GP_TRADEREADY, (BYTE*)strPacket, stream1.GetLength());
		}
	}
}

void CGameManager::TradeSuccess(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);
	_tagItemInfo Item = {};
	int iOtherPlayerCharacterNumber = 0;

	CStream stream1; // 보내는용
	char strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	

	//int ItemCount = 0;
	//stream.Read(&ItemCount, sizeof(int));

	//stream1.Write(&ItemCount, sizeof(int));

	//for (int i = 0; i < ItemCount; ++i)
	//{
	//	char ItemName[32] = {};
	//	stream.Read(ItemName, sizeof(ItemName));
	//	ItemType eType = ItemType:: IT_END; //크기는 4바이트인데 메모리복사하는건 1바이트니 값오류
	//	stream.Read(&eType, sizeof(bool));
	//	int iPrice;
	//	stream.Read(&iPrice, sizeof(int));
	//	int iSell;
	//	stream.Read(&iSell, sizeof(int));

	//	int iOptionCount = 0;
	//	stream.Read(&iOptionCount, sizeof(iOptionCount));
	//	for (int j = 0; j < iOptionCount; j++)
	//	{
	//		_tagItemOption tOption;
	//		stream.Read(&tOption, sizeof(tOption));

	//		Item.vecOption.push_back(tOption);
	//	}

	//	stream1.Write(ItemName, sizeof(ItemName));
	//	stream1.Write(&eType, sizeof(bool));
	//	stream1.Write(&iPrice, sizeof(int));
	//	stream1.Write(&iSell, sizeof(int));

	//	stream1.Write(&iOptionCount, sizeof(int));
	//	for (int j = 0; j < iOptionCount; j++)
	//	{
	//		stream1.Write(&Item.vecOption[j], sizeof(_tagItemOption));
	//	}
	//}

	// =  =========================================

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			(*iter)->WritePacket(GP_TRADESUCCESS, (BYTE*)strPacket, stream1.GetLength());

			break;
		}
	}
}

void CGameManager::GroupCreate(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);
	// 방장이라서 맨처음에  넣어준다.
	pSession->AddGroupList(pSession);
	//auto GroupAdmin = pSession->GetGroupList();
	//GroupAdmin.push_back(pSession);
}

void CGameManager::GroupRequest(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iOtherPlayerCharacterNumber = 0;
	stream.Read(&iOtherPlayerCharacterNumber, sizeof(int));

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterEnd = pUserList->end();
	for (; iter != iterEnd; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == iOtherPlayerCharacterNumber)
		{
			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);
			int RequestCharacterNumber = pSession->GetCharacterNumber();
			char RequestPlayerName[NAME_SIZE] = {};
			memcpy(RequestPlayerName, pSession->GetstrName(), NAME_SIZE);

			stream1.Write(&RequestCharacterNumber, sizeof(int));
			stream1.Write(RequestPlayerName, NAME_SIZE);

			(*iter)->WritePacket(GP_GROUPREQUEST, (BYTE*)strPacket, stream1.GetLength());

			break;
		}
	}
}

void CGameManager::GroupAccept(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int RequestCharacterNumber = 0;
	stream.Read(&RequestCharacterNumber, sizeof(int));

	const list<CUser*>* pUserList = GET_SINGLE(CUserManager)->GetConnectUserList();
	auto iter = pUserList->begin();
	auto iterend = pUserList->end();
	for (; iter != iterend; ++iter)
	{
		if ((*iter)->GetCharacterNumber() == RequestCharacterNumber)
		{
			auto GroupList = (*iter)->GetGroupList();


			CStream stream1;
			char strPacket[MAX_BUFFER_LENGTH] = {};
			stream1.SetBuffer(strPacket);
			int iAcceptCharacterNumber = pSession->GetCharacterNumber();
			char AcceptCharacterName[NAME_SIZE] = {};
			memcpy(AcceptCharacterName, pSession->GetstrName(), NAME_SIZE);
			char AcceptCharacterJob[NAME_SIZE] = {};
			memcpy(AcceptCharacterJob, pSession->GetChracterInfo().strJob, NAME_SIZE);
			int iAcceptCharacterLevel = 0;
			iAcceptCharacterLevel = pSession->GetChracterInfo().iLevel;

			stream1.Write(&iAcceptCharacterNumber, sizeof(int));
			stream1.Write(AcceptCharacterName, NAME_SIZE);
			stream1.Write(AcceptCharacterJob, NAME_SIZE);
			stream1.Write(&iAcceptCharacterLevel, sizeof(int));

			(*iter)->WritePacket(GP_GROUPACCEPT, (BYTE*)strPacket, stream1.GetLength());

			// 여기서 각 파티원들에게 파티수락한 유저의 정보를 추가시키고 
			// 파티수락한 유저는 기존에 있던 파티원들 정보를 받아가야한다.

			CStream stream2;
			//strPacket[MAX_BUFFER_LENGTH] = {}; // 재사용 불가
			char strPacket2[MAX_BUFFER_LENGTH] = {};
			stream2.SetBuffer(strPacket2);

			int iRequestCharacterNumber = (*iter)->GetCharacterNumber();
			char RequestCharacterName[NAME_SIZE] = {};
			memcpy(RequestCharacterName, (*iter)->GetstrName(), NAME_SIZE);
			char RequestCharacterJob[NAME_SIZE] = {};
			memcpy(RequestCharacterJob, (*iter)->GetChracterInfo().strJob, NAME_SIZE);
			int iRequestCharacterLevel = (*iter)->GetChracterInfo().iLevel;

			stream2.Write(&iRequestCharacterNumber, sizeof(int));
			stream2.Write(RequestCharacterName, NAME_SIZE);
			stream2.Write(RequestCharacterJob, NAME_SIZE);
			stream2.Write(&iRequestCharacterLevel, sizeof(int));

			pSession->WritePacket(GP_GROUPACCEPT, (BYTE*)strPacket2, stream2.GetLength());


			(*iter)->AddGroupList(pSession);
			break;
		}
	}
}

void CGameManager::GroupCancel(BYTE* pPacket, CUser* pSession)
{
}

void CGameManager::GroupLeave(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	auto GroupList = pSession->GetGroupList();
	auto iter = GroupList->begin();
	auto iterend = GroupList->end();
	for (; iter != iterend; ++iter)
	{
		if ((*iter) == pSession)
			continue;

		CStream stream1;
		char strPacket[MAX_BUFFER_LENGTH] = {};
		stream1.SetBuffer(strPacket);

		(*iter)->WritePacket(GP_GROUPLEAVE, (BYTE*)strPacket, stream1.GetLength());
	}

}

void CGameManager::GroupKick(BYTE* pPacket, CUser* pSession)
{
}

void CGameManager::GroupSuccess(BYTE* pPacket, CUser* pSession)
{
}

void CGameManager::PlayerAttackDamage(BYTE* pPacket, CUser* pSession)
{
	CStream stream;
	stream.SetBuffer((char*)pPacket);

	int iAttackDamage = 0;
	int iMonsterSerial = 0;

	stream.Read(&iMonsterSerial, sizeof(int));
	stream.Read(&iAttackDamage, sizeof(int));
	
	CLevel* pLevel = pSession->GetLevel();
	pLevel->PlayerAttack(pSession, iAttackDamage, iMonsterSerial);

	// 이 유저가 몬스터를 공격해야 한다. 여기로는 단순하게 공격한다는 신호, 데미지만 온것이다.
	// 때릴 대상도 여기에서 찾아야 한다.
}

void CGameManager::MonsterAttackDamage(BYTE* pPacket, CUser* pSession)
{
	// 여기서 스킬인지 평타인지 구분을 하고 적용을 해줘야하는데 그러면
	// 이 서버에서 몬스터의 스킬이나 평타 데미지를 전부 알고있어야하는건데
	// 클라에서 서버로 데미지를 보내는것보다 서버가 각 데미지를 알고있는것이
	// 더 효율적으로 작용할수 있을거 같다.
	// 그러면 여기서는 평타인지 스킬몇인지 구분만 하고
	// 플레이어나 다른 유저에 데미지 계산을 적용하고 각 유저와 플레이어에게
	// 보내주자.

	// 그런데 일반 몬스터랑 보스몬스터를 구분을 지어야하는건가.....
	// 그냥 통일해서 적용을 하는것이 좋아 보인다.
	// 그리고 플레이어의 공격타입인 enum AttackType을 혼용해서 쓸까
	// 아니면 따로 구분을 지어서 쓸까 그러면 enum class로 적용을 하는것이 좋아보이겠지?

}

void CGameManager::MainPortal(BYTE* pPacket, CUser* pSession)
{
	// 들어온다.
	// 전송받은 레벨타입을 얻어와서 해당 레벨로 유저의 레벨을 변경시키고
	// 레벨전환 승인 패킷을 보내주도록 한다.

	ELevel LevelType  = ELevel::None;

	CStream	stream;
	stream.SetBuffer((char*)pPacket);
	stream.Read(&LevelType, sizeof(ELevel));

	// 이 LevelType은 이동해야할 레벨이다.
	// 보스룸에서 시작되는 위치정보를 넘겨주어서 보스룸에서의 영역을 구해서 넘겨줄 수 있도록 해야 한다.
	switch (LevelType)
	{
	case ELevel::MainLevel:
		break;
	case ELevel::DesertLevel:
		break;
	case ELevel::SevarogBossRoom:
		pSession->SetPos(-1140.f, 0.f, 724.f);
		pSession->SetPrevPos(-1140.f, 0.f, 724.f);

		cout << pSession->GetstrName() << "보스룸 으로 맵 이동하였습니다..." << endl;
		break;
	}
	pSession->SetLevelType(LevelType);
	
	// 기존 레벨타입에서 제거
	CLevel* pLevel = pSession->GetLevel();
	pLevel->DeleteUser(pSession);

	//// Read한 레벨타입에 추가
	pLevel = CLevelManager::GetInst()->FindLevel(LevelType);
	pLevel->AddUser(pSession);

	CStream	 stream1;
	char strPacket[MAX_BUFFER_LENGTH] = {};
	stream1.SetBuffer(strPacket);

	bool	bCreation = true;
	stream1.Write(&bCreation, sizeof(bool));

	pSession->WritePacket(GP_MAINPORTAL, (BYTE*)strPacket, stream1.GetLength());

	// 다음 레벨에 있는 몬스터 정보를 넘겨준다.

	{
		pLevel->SendMonsterInfo(pSession);

		// -------------------------------------------------
		CStream	stream1;
		stream1.SetBuffer((char*)pPacket);

		int iCharacterNumber = pSession->GetCharacterNumber();
		stream1.Write(&iCharacterNumber, sizeof(int));

		int	iJob = pSession->GetiJob();
		stream1.Write<int>(&iJob, sizeof(int));

		char	strJobName[JOBNAME_SIZE] = {};
		stream1.Write(strJobName, JOBNAME_SIZE);

		_tagCharacterInfo CharInfo = pSession->GetChracterInfo();
		stream1.Write(&CharInfo, sizeof(_tagCharacterInfo));

		pSession->WritePacket(GP_GAMEIN, (BYTE*)strPacket, stream1.GetLength());
	}
}

void CGameManager::FindPath(BYTE* pPacket, CPacketSession* pSession)
{
	CStream	stream;

	stream.SetBuffer((char*)pPacket);

	bool	Find = false;
	stream.Read(&Find, sizeof(bool));

	int	MonsterNumber = 0;
	stream.Read(&MonsterNumber, sizeof(int));
	 
	ELevel	LevelType;
	stream.Read(&LevelType, sizeof(ELevel));

	if (Find)
	{
		int	PathCount = 0;
		stream.Read(&PathCount, sizeof(int));

		std::list<Vector3>	PathList;

		for (int i = 0; i < PathCount; ++i)
		{
			Vector3	Point;
			stream.Read(&Point, sizeof(Vector3));
			
			PathList.push_back(Point);

			//CMonster* Monster = CMonsterManager::GetInst()->FindMonster(MonsterNumber);

			//if (Monster)
			//	Monster->SetPath(vecPath);
		}

		// 레벨을 얻어온다.
		CLevel* Level = CLevelManager::GetInst()->FindLevel(LevelType);

		if (Level)
		{
			CMonster* Monster = Level->FindMonster(MonsterNumber);

			if (Monster)
				Monster->SetPath(PathList);
		}
	}
}

void CGameManager::AlivePacket(CUser* pSession)
{
	pSession->ClearAliveReceive();
}
