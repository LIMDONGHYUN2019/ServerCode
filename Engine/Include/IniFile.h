#pragma once

#include "Engine.h"

class CIniFile
{
public:
	CIniFile();
	virtual ~CIniFile();

private:
	TCHAR	m_strFileName[MAX_PATH];

public:
	bool Open(const TCHAR* pFileName);
	bool Close();

	bool SetValue(const TCHAR* pKeyName, const TCHAR* pValueName, const TCHAR* pValue);
	bool SetValue(const TCHAR* pKeyName, const TCHAR* pValueName, unsigned int iValue);
	bool SetValue(const TCHAR* pKeyName, const TCHAR* pValueName, float fValue);

	bool GetValue(const TCHAR* pKeyName, const TCHAR* pValueName, TCHAR* pValue, unsigned int* pLength);
	bool GetValue(const TCHAR* pKeyName, const TCHAR* pValueName, unsigned int* pValue);
	bool GetValue(const TCHAR* pKeyName, const TCHAR* pValueName, float* pValue);
};

