
#include <iostream>
#include "NetworkManager.h"
#include "MiniDump.h"

#ifdef _DEBUG

#ifdef _WIN64
#pragma comment(lib, "NetworkEngine_Debug.lib")
#else
#pragma comment(lib, "NetworkEngine32_Debug.lib")
#endif // _WIN64

#else

#ifdef _WIN64
#pragma comment(lib, "NetworkEngine.lib")
#else
#pragma comment(lib, "NetworkEngine32.lib")
#endif

#endif // _DEBUG


using namespace std;

int main()
{
	// COM ��ü�� ����ϱ� ���ؼ� �ʱ�ȭ�Ѵ�.
	// �� �Լ��� �� �����帶�� ���� �Ѵ�.
	CoInitialize(nullptr);

	if (!CMiniDump::Begin())
		return 0;

	if (!GET_SINGLE(CNetworkManager)->Init())
	{
		DESTROY_SINGLE(CNetworkManager);
		return 0;
	}

	GET_SINGLE(CNetworkManager)->Run();

	DESTROY_SINGLE(CNetworkManager);

	CMiniDump::End();

	CoUninitialize();

	return 0;
}
