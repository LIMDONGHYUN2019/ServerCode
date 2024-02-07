#pragma once

#include "GameInfo.h"
#include "Network/NetSession.h"
#include "CircleQueue.h"

class CGameManager
{
private:
	class CGameIocp*									m_pIocp;

	class CNetSession*									m_pListen;
	class CPacketSession*								m_pAISession;

	vector<_tagCharacterInfo*>							m_vecCharInfo;
	vector<_tagItemInfo*>								m_vecStoreInfo;
	CCircleQueue<PacketQueueData, 1000>	m_PacketQueue;

	//unordered_map<FieldMap, class CGameAreaManager*> m_vecMap;

	class CTimer*	m_pTimer;
	bool			m_bLoop;

public:
	class CPacketSession* GetAISession()	const
	{
		return m_pAISession;
	}

	class CNetSession* GetListenSession()	const
	{
		return m_pListen;
	}

	CCircleQueue<PacketQueueData, 1000>* GetPacketQueue()
	{
		return &m_PacketQueue;
	}

	const vector<_tagCharacterInfo*>* GetCharInfo()	const
	{
		return &m_vecCharInfo;
	}

	const vector<_tagItemInfo*>* GetStoreInfo()	const
	{
		return &m_vecStoreInfo;
	}

	/*const unordered_map<FieldMap, class CGameAreaManager*>* GetMapList()	const
	{
		return &m_vecMap;
	}*/

private:
	void Connect(BYTE* pPacket, class CUser* pSession);
	void DisConnect(BYTE* pPacket, class CUser* pSession);

	void CreateCharacter(BYTE* pPacket, class CUser* pSession);
	void SelectCharacterInfo(BYTE* pPacket, class CUser* pSession);
	void CreateCharacterInfo(BYTE* pPacket, class CUser* pSession);
	void DeleteCharacterInfo(BYTE* pPacket, class CUser* pSession);

	void GameStart(BYTE* pPacket, class CUser* pSession, int iLength);
	void GameIn(BYTE* pPacket, class CUser* pSession);

	void UserAppear(BYTE* pPacket, class CUser* pSession);
	void MoveStart(BYTE* pPacket, class CUser* pSession);
	void Move(BYTE* pPacket, class CUser* pSession);
	void MoveEnd(BYTE* pPacket, class CUser* pSession);
	void RotationPacket(BYTE* pPacket, class CUser* pSession);

	void OtherAttack(BYTE* pPacket, class CUser* pSession);
	void OtherAttack(BYTE* pPacket, int Length, class CUser* pSession);
	void OtherSkill1(BYTE* pPacket, class CUser* pSession);
	void OtherSkill2(BYTE* pPacket, class CUser* pSession);

	void LevelStart(BYTE* pPacket, class CUser* pSession);

	void CharacterLevelUp(BYTE* pPacket, class CUser* pSession);

	void MonsterItemDrop(BYTE* pPacket, class CUser* pSession);

	void SkillList(BYTE* pPacket, class CUser* pSession);
	void SkillSave(BYTE* pPacket, class CUser* pSession);
	void SkillUp(BYTE* pPacket, class CUser* pSession);

	void StoreList(BYTE* pPacket, class CUser* pSession);

	void InventoryList(BYTE* pPacket, class CUser* pSession);
	void InventoryAdd(BYTE* pPacket, class CUser* pSession);
	void InventoryDelete(BYTE* pPacket, class CUser* pSession);
	void InventoryGold(BYTE* pPacket, class CUser* pSession);

	void TradeRequest(BYTE* pPacket, class CUser* pSession);
	void TradeRequestAccept(BYTE* pPacket, class CUser* pSession);
	void TradeCancel(BYTE* pPacket, class CUser* pSession);
	void TradeAdd(BYTE* pPacket, class CUser* pSession);
	void TradeDelete(BYTE* pPacket, class CUser* pSession);
	void TradeReady(BYTE* pPacket, class CUser* pSession);
	void TradeSuccess(BYTE* pPacket, class CUser* pSession);

	void GroupCreate(BYTE* pPacket, class CUser* pSession);
	void GroupRequest(BYTE* pPacket, class CUser* pSession);
	void GroupAccept(BYTE* pPacket, class CUser* pSession);
	void GroupCancel(BYTE* pPacket, class CUser* pSession);
	void GroupLeave(BYTE* pPacket, class CUser* pSession);
	void GroupKick(BYTE* pPacket, class CUser* pSession);
	void GroupSuccess(BYTE* pPacket, class CUser* pSession);

	void PlayerAttackDamage(BYTE* pPacket, class CUser* pSession);
	void MonsterAttackDamage(BYTE* pPacket, class CUser* pSession);
	void MainPortal(BYTE* pPacket, class CUser* pSession);
	void FindPath(BYTE* pPacket, class CPacketSession* pSession);
	void AlivePacket(class CUser* pSession);


public:
	bool Init();
	void Run();
	_tagCharacterInfo* FindCharacterInfo(const char* job);
	DECLARE_SINGLE(CGameManager)
};

