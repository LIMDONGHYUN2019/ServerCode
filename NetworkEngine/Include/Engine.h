#pragma once

#define	_WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <list>
#include <unordered_map>
#include <vector>
#include <process.h>
#include <tchar.h>
#include <assert.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <DbgHelp.h>
#include <string>
#include <utility>

using namespace std;

//#import "C:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" rename("EOF", "EndOfFile") no_namespace

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#pragma warning(disable : 4101)

#include "Macro.h"
#include "Flag.h"
#define	MAX_BUFFER_LENGTH	4084
#define	NAME_SIZE		64

#define	ROOT_PATH	"RootPath"
#define	DATA_PATH	"DataPath"

enum IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE
};

typedef struct _tagOverlappedEx
{
	OVERLAPPED	tOverlapped;
	IO_TYPE		eIOType;
	void* pObject;
}OverlappedEx, *POverlappedEx;

typedef struct _tagSessionQueueData
{
	void* pObject;
	BYTE	Data[MAX_BUFFER_LENGTH];
	DWORD	dwLength;
	
	char	cRemoteAddr[32];
	USHORT	sRemotePort;

	DWORD	dwProtocol;
}SessionQueueData, *PSessionQueueData;
// IOCP 데이터 처리 큐
// 즉, IOCP전용 큐를 위한 데이터를 구조체로 변경한것.
