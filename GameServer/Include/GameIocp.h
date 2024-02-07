#pragma once

#include "GameInfo.h"
#include "Network/Iocp.h"

class CGameIocp :
	public CIocp
{
public:
	CGameIocp();
	virtual ~CGameIocp();

private:
	class CNetSession* m_pListenSession;
	class CUserManager* m_pUserManager;

public:
	void SetListenSession(class CNetSession* pSession)
	{
		m_pListenSession = pSession;
	}

public:
	virtual bool Begin();
	virtual bool End();
	void ConnectSession(class CPacketSession* Session);

protected:
	virtual void OnIoRead(void* pObj, unsigned int iLength);
	virtual void OnIoWrite(void* pObj, unsigned int iLength);
	virtual void OnIoConnected(void* pObj);
	virtual void OnIoDisconnected(void* pObj);

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

	void OtherAttack(BYTE* pPacket, class CUser* pSession);
	void OtherSkill1(BYTE* pPacket, class CUser* pSession);
	void OtherSkill2(BYTE* pPacket, class CUser* pSession);

	void LevelStart(BYTE* pPacket, class CUser* pSession);
	void StoreList(BYTE* pPacket, class CUser* pSession);

	void InventoryList(BYTE* pPacket, class CUser* pSession);
	void InventoryAdd(BYTE* pPacket, class CUser* pSession);
	void InventoryDelete(BYTE* pPacket, class CUser* pSession);

	void TradeRequest(BYTE* pPacket, class CUser* pSession);
	void TradeRequestAccept(BYTE* pPacket, class CUser* pSession);
	void TradeCancel(BYTE* pPacket, class CUser* pSession);
	void TradeAdd(BYTE* pPacket, class CUser* pSession);
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

	void SevarogRoomCreate(BYTE* pPacket, class CUser* pSession);
};

