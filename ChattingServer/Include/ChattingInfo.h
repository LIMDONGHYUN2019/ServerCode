#pragma once

#include "Engine.h"

#define	CHATTING_PORT	6383
#define	MAX_USER		100
#define	USERNAME_SIZE	64
#define	MESSAGE_LENGTH	128

enum CHATTING_PROTOCOL
{
	CP_LOGIN,
	CP_LOGOUT,
	CP_MESSAGE,
};

typedef struct _tagChattingInfo
{
	char	strName[USERNAME_SIZE];
	char	strPass[MESSAGE_LENGTH];
}ChattingInfo, * PChattingInfo;
