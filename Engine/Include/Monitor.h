#pragma once
#include "MonitorHeader.h"

class Monitor
{
private:
	ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	int numProcessors;
	HANDLE self;
	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;
	wchar_t CpuName[255];//cpu 이름 O
	DWORD CpuSpeed;	//cpu 속도 O 기본속도
	HKEY hKey;
	DWORD cname_size;
	DWORD cspeed_size;

public:
	Monitor();
	~Monitor();
	double processCpuUsage()
	{
		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		double percent;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		percent = (double)((sys.QuadPart - lastSysCPU.QuadPart) + (user.QuadPart - lastUserCPU.QuadPart));
		percent /= (now.QuadPart - lastCPU.QuadPart);
		percent /= numProcessors;
		lastCPU = now;
		lastUserCPU = user;
		lastSysCPU = sys;
		percent = percent * 100;
		return fixInRange(0, percent, 100);
	}

	DWORDLONG FullRam()
	{
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
		return totalPhysMem;
	}

	SIZE_T processMemUsage()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return (size_t)pmc.WorkingSetSize;
	}

	void InitReg() {
		cname_size = sizeof(CpuName);
		cspeed_size = sizeof(DWORD);

		RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);
		RegQueryValueEx(hKey, L"ProcessorNameString", NULL, NULL, (LPBYTE)CpuName, &cname_size);
		RegQueryValueEx(hKey, L"~MHz", NULL, NULL, (LPBYTE)&CpuSpeed, &cspeed_size); // CPU Speed
		RegCloseKey(hKey);
	}
	void InitCPUName() {

		//GetSystemInfo 함수를 이용해 논리적 코어 개수를 얻어냅니다.
		wchar_t num[8];
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		swprintf(num, 8, L" * %d", systemInfo.dwNumberOfProcessors);
		wcscat_s(CpuName, 100, num);
	}
	wstring GetCpuName() {
		return CpuName;
	}
	double GetCpuSpeed() {
		return CpuSpeed / 1000.0;
	}

	SIZE_T physyicMemUsage()
	{
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		return (size_t)memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	}

	void init() {
		PdhOpenQuery(NULL, NULL, &cpuQuery);
		// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
		PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
		PdhCollectQueryData(cpuQuery);
	}

	double getCurrentValue() {
		PDH_FMT_COUNTERVALUE counterVal;

		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		return counterVal.doubleValue;
	}
	DWORD PagedFalut()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		return pmc.PageFaultCount;
	}

	void Time()
	{
		system_clock::time_point StartTime = system_clock::now();
		system_clock::time_point EndTime = system_clock::now();
		while (1)
		{
			auto duration = chrono::duration_cast<chrono::milliseconds>(EndTime - StartTime);
			if (duration.count() > 1500)
				break;
			EndTime = system_clock::now();
		}
	}
};
