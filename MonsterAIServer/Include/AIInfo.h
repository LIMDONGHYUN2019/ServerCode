#pragma once

#include "Engine.h"
#include "Vector3.h"
#include <algorithm>

#define	GAMESERVER_IP	"127.0.0.1"
#define	GAMESERVER_PORT	6381

#define	ROOMSERVER_IP	"127.0.0.1"
#define	ROOMSERVER_PORT	6382

#define	CHATTINGSERVER_IP	"127.0.0.1"
#define	CHATTINGSERVER_PORT	6383

#define	AISERVER_IP	"127.0.0.1"
#define	AISERVER_PORT	6384

#define	MONITORINGSERVER_IP "127.0.0.1"
#define	MONITORINGSERVER_PORT	6385

enum class NavCell_Type
{ // 못가는곳, 둔화되는곳, 피해입는곳 등등
	Normal,
	Wall
};

enum class NavNode_Type
{
	None,
	Open,
	Close
};

enum AI_PROTOCOL
{
	AP_FINDPATH = 55
};

struct PacketQueueData
{
	int	Protocol;
	int	Length;
	BYTE	Packet[MAX_BUFFER_LENGTH];
	class CPacketSession* pSession;

	PacketQueueData() :
		Protocol(-1),
		Length(0),
		Packet{},
		pSession(nullptr)
	{
	}
};

enum class ELevel
{
	None,
	Kingdom,
	Desert,
	SevarogBossRoom
};

struct NavWorkData
{
	class CPacketSession* Session;
	int	MonsterNumber;	// 어떤 몬스터가 길찾기를 수행하는지를 저장한다.
	ELevel	LevelType;
	Vector3	StartPos;
	Vector3	EndPos;
};

enum JumpPoint_Dir
{
	JPD_TOP,
	JPD_TL,
	JPD_LEFT,
	JPD_BL,
	JPD_BOTTOM,
	JPD_BR,
	JPD_RIGHT,
	JPD_TR,
	JPD_MAX
};
