
#include <iostream>
#include "LoginManager.h"
#include "MiniDump.h"

#ifdef _DEBUG

#ifdef _WIN64
#pragma comment(lib, "NetworkEngine_Debug.lib")
#pragma comment(lib, "DataBase_Debug.lib")
#pragma comment(lib, "Monitor_Debug.lib")
#else
#pragma comment(lib, "NetworkEngine32_Debug.lib")
#pragma comment(lib, "DataBase32_Debug.lib")
#pragma comment(lib, "Monitor32_Debug.lib")
#endif // _WIN64

#else

#ifdef _WIN64
#pragma comment(lib, "NetworkEngine.lib")
#pragma comment(lib, "DataBase.lib")
#pragma comment(lib, "Monitor.lib")
#else
#pragma comment(lib, "NetworkEngine32.lib")
#pragma comment(lib, "DataBase32.lib")
#pragma comment(lib, "Monitor32.lib")
#endif

#endif // _DEBUG


using namespace std;

int main()
{
	// COM 객체를 사용하기 위해서 초기화한다.
	// 이 함수는 각 스레드마다 들어가야 한다.
	CoInitialize(nullptr);

	if (!CMiniDump::Begin())
		return 0;

	if (!GET_SINGLE(CLoginManager)->Init())
	{
		DESTROY_SINGLE(CLoginManager);
		return 0;
	}

	GET_SINGLE(CLoginManager)->Run();

	DESTROY_SINGLE(CLoginManager);

	CMiniDump::End();

	CoUninitialize();

	return 0;
}
