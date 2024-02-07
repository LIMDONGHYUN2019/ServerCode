#include "MiniDump.h"

typedef BOOL(__stdcall* MiniDumpProc)(
	HANDLE hProcess,
	DWORD dwPid,
	HANDLE	hFile,
	MINIDUMP_TYPE DumpType,
	const PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	const PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	const PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER	PrevExceptionFilter = nullptr;

LONG __stdcall UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo)
{
	HMODULE	hModule = nullptr;

	// dll�� �о�´�.
	hModule = LoadLibrary(TEXT("DBGHELP.DLL"));

	if (hModule)
	{
		// dll���� �����Ǵ� �Լ��� �̿��Ͽ� ������ �����.
		// MiniDumpWriteDump �� �Լ��� dll�� �ִµ� �� �Լ��� �ּҸ�
		// ���ͼ� ȣ������ �� �ְ� ���ش�.
		// �� ����� �������̴�.
		MiniDumpProc	pDump = (MiniDumpProc)GetProcAddress(hModule, "MiniDumpWriteDump");

		if (pDump)
		{
			TCHAR	strDumpPath[MAX_PATH] = {};
			SYSTEMTIME	tSystemTime;

			GetLocalTime(&tSystemTime);

			_sntprintf_s(strDumpPath, MAX_PATH,
				TEXT("%d-%d-%d %d_%d_%d.dmp"),
				tSystemTime.wYear,
				tSystemTime.wMonth,
				tSystemTime.wDay,
				tSystemTime.wHour,
				tSystemTime.wMinute,
				tSystemTime.wSecond);

			HANDLE	hFile = CreateFile(strDumpPath,
				GENERIC_WRITE, FILE_SHARE_WRITE,
				nullptr, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr);

			if (hFile != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION	DumpExceptionInfo = {};

				DumpExceptionInfo.ThreadId = GetCurrentThreadId();
				DumpExceptionInfo.ExceptionPointers = pExceptionInfo;
				DumpExceptionInfo.ClientPointers = FALSE;

				BOOL bSuccess = pDump(GetCurrentProcess(),
					GetCurrentProcessId(),
					hFile,
					MiniDumpNormal,
					&DumpExceptionInfo,
					nullptr, nullptr);

				if (bSuccess)
				{
					CloseHandle(hFile);
					return EXCEPTION_EXECUTE_HANDLER;
				}
			}

			CloseHandle(hFile);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

bool CMiniDump::Begin()
{
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// ������ ������ ���͸� �޾ƿ��� �� ���͸� �����Ѵ�.
	PrevExceptionFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);

	return true;
}

bool CMiniDump::End()
{
	// ������ ���ͷ� �ٽ� �����ش�.
	SetUnhandledExceptionFilter(PrevExceptionFilter);

	return true;
}
