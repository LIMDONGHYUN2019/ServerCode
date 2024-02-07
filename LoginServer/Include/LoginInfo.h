#pragma once

#include"Engine.h"

#define	MAX_USER	100
#define	EMAIL_SIZE	128
#define	ID_SIZE		64
#define	PASS_SIZE	64

#define	LOGIN_PORT	6380

#define	GAMESERVER_IP	"127.0.0.1"
#define	GAMESERVER_PORT	6381

#define	ROOMSERVER_IP	"127.0.0.1"
#define	ROOMSERVER_PORT	6382

#define	CHATTINGSERVER_IP	"127.0.0.1"
#define	CHATTINGSERVER_PORT	6383

#define	MONITORINGSERVER_IP	"127.0.0.1"
#define	MONITORINGSERVER_PORT	6385

enum LOGIN_PROTOCOL
{
	LP_MEMBERSHIP,
	LP_LOGIN,
	LP_LOGOUT,
	LP_LOGIN_SUCCESS,
	LP_LOGIN_FAIL,
	LP_MEMBERSHIP_SUCCESS,
	LP_MEMBERSHIP_FAIL
};

typedef struct _tagMemberInfo
{
	char	strName[NAME_SIZE];
	char	strEmail[EMAIL_SIZE];
	char	strID[ID_SIZE];
	char	strPass[PASS_SIZE];
}MemberInfo, *PMemberInfo;

enum MonitoringList
{
	LoginServer,
	GameServer,
	ChattingServer
};