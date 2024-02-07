#pragma once
#include <iostream>
#include <WinSock2.h>
#include <TCHAR.h>
#include "pdh.h"
#include "psapi.h"
#include <chrono>

#pragma comment(lib, "pdh.lib")
using namespace std;
using namespace chrono;

//범위 보정 및 체크
#define fixInRange(minimum, x, maximum)     min(maximum, max(x, minimum)) 
#define isInRange(minimum, x, maximum)      (x == fixInRange(minimum, x, maximum)) ? true : false
