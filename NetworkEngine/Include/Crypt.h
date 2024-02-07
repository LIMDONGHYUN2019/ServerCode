#pragma once

#include "Engine.h"

class CCrypt
{
public:
	static bool EnCrypt(char* pSrc, char* pDest, int iLength);
	static bool DeCrypt(char* pSrc, char* pDest, int iLength);
};

