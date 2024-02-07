#pragma once

#include "Engine.h"

class CMiniDump
	// 릴리즈 모드에서도 작동할려면 따로 작업을 해야함.
{
public:
	static bool Begin();
	static bool End();
};

