#pragma once

#include "Engine.h"

class CLog
{
public:
	static bool WriteLog(TCHAR* pData, ...)
	{
		SYSTEMTIME	tTime = {};

		TCHAR	strCurrentData[128] = {};
		TCHAR	strCurrentFileName[MAX_PATH] = { };
		FILE* pFile = nullptr;
		TCHAR	strDebugLog[MAX_BUFFER_LENGTH] = {};

		va_list	ap;
		TCHAR	strLog[MAX_BUFFER_LENGTH] = {};

		va_start(ap, pData);
		_vstprintf_s(strLog, pData, ap);
		va_end(ap);
		
		GetLocalTime(&tTime);
		wsprintf(strCurrentData, TEXT("%d-%d-%d %d:%d:%d"), tTime.wYear, tTime.wMonth, tTime.wDay,
			tTime.wHour, tTime.wMinute, tTime.wSecond);

		wsprintf(strCurrentFileName, TEXT("LOG_%d-%d-%d %d.log"), tTime.wYear, tTime.wMonth, tTime.wDay,
			tTime.wHour);

		_tfopen_s(&pFile, strCurrentFileName, TEXT("a"));

		if (!pFile)
			return false;

		_ftprintf(pFile, TEXT("[%s] %s\n"), strCurrentData, strLog);
		wsprintf(strDebugLog, TEXT("[%s] %s\n"), strCurrentData, strLog);


		fflush(pFile);
		fclose(pFile);

		OutputDebugString(strDebugLog);
		_tprintf(TEXT("%s"), strDebugLog);
		

		return true;
	}

	static bool WriteLogNoData(TCHAR* pData, ...)
	{
		SYSTEMTIME	tTime = {};

		TCHAR	strCurrentFileName[MAX_PATH] = { };
		FILE* pFile = nullptr;
		TCHAR	strDebugLog[MAX_BUFFER_LENGTH] = {};

		va_list	ap;
		TCHAR	strLog[MAX_BUFFER_LENGTH] = {};

		va_start(ap, pData);
		_vstprintf_s(strLog, pData, ap);
		va_end(ap);

		GetLocalTime(&tTime);

		wsprintf(strCurrentFileName, TEXT("LOG_%d-%d-%d %d.log"), tTime.wYear, tTime.wMonth, tTime.wDay,
			tTime.wHour);

		_tfopen_s(&pFile, strCurrentFileName, TEXT("a"));

		if (!pFile)
			return false;

		_ftprintf(pFile, TEXT("%s"), strLog);
		wsprintf(strDebugLog, TEXT("%s"), strLog);

		fflush(pFile);

		fclose(pFile);

		OutputDebugString(strDebugLog);
		_tprintf(TEXT("%s"), strDebugLog);

		return true;
	}
};

