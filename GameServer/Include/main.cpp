
#include <iostream>
#include "Service.h"
#include "MiniDump.h"
#include "GameManager.h"

using namespace std;

#ifdef _DEBUG

#ifdef _WIN64
#pragma comment(lib, "NetworkEngine_Debug.lib")
#pragma comment(lib, "DataBase_Debug.lib")
#pragma comment(lib, "Math_Debug.lib")
#pragma comment(lib, "Monitor_Debug.lib")
#else
#pragma comment(lib, "NetworkEngine32_Debug.lib")
#pragma comment(lib, "DataBase32_Debug.lib")
#pragma comment(lib, "Math32_Debug.lib")
#pragma comment(lib, "Monitor32_Debug.lib")
#endif // _WIN64

#else

#ifdef _WIN64
#pragma comment(lib, "NetworkEngine.lib")
#pragma comment(lib, "DataBase.lib")
#pragma comment(lib, "Math.lib")
#pragma comment(lib, "Monitor.lib")
#else
#pragma comment(lib, "NetworkEngine32.lib")
#pragma comment(lib, "DataBase32.lib")
#pragma comment(lib, "Math32.lib")
#pragma comment(lib, "Monitor32.lib")
#endif

#endif // _DEBUG

#define	GAMESERVER_PORT	6381

UINT __stdcall TestThread(void* pArg)
{
	return 0;
}

class CServiceTest :
	public CService //���
{
private:
	HANDLE		m_hThread;

public:
	virtual void OnStart()
	{
		m_hThread	= (HANDLE)_beginthreadex(nullptr, 0,
			TestThread, nullptr, 0, 0);
	}

	virtual void OnStop()
	{
		CloseHandle(m_hThread);
	}

public:
	void Main()
	{
		Begin(TEXT("TestService"));
		// ��� �����·� ������ش�.(����ɶ�����)
		getchar();
		End();
	}
};

int main()
{
	// COM ��ü�� ����ϱ� ���ؼ� �ʱ�ȭ�Ѵ�.
	// �� �Լ��� �� �����帶�� ���� �Ѵ�.
	CoInitialize(nullptr);

	//SetConsoleOutputCP(CP_UTF8);

	if (!CMiniDump::Begin())
		return 0;

	if (!GET_SINGLE(CGameManager)->Init())
	{
		DESTROY_SINGLE(CGameManager);
		return 0;
	}

	GET_SINGLE(CGameManager)->Run();

	DESTROY_SINGLE(CGameManager);

	CMiniDump::End();

	CoUninitialize();

	return 0;
}
