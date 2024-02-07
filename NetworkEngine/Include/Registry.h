#pragma once

#include "Engine.h"

class CRegistry
{
public:
	CRegistry();
	virtual ~CRegistry();

private:
	HKEY	m_pRootKey;
	bool	m_bOpen;

public:
	bool Open(HKEY pRootKey, const TCHAR* pSubKey);
	bool Close();
	bool CreateKey(HKEY pRootKey, const TCHAR* pSubKey);
	bool DeleteKey(HKEY pRootKey, const TCHAR* pSubKey);
	bool SetValue(const TCHAR* pName, const TCHAR* pValue);
	bool SetValue(const TCHAR* pName, unsigned int dwValue);
	bool SetValueForMultiSz(const TCHAR* pName, const TCHAR* pValue, unsigned int dwLength);
	bool GetValue(const TCHAR* pName, const TCHAR* pValue, unsigned int* pLength);
	bool GetValue(const TCHAR* pName, const unsigned int* pValue);
	bool DeleteValue(const TCHAR* pName);
};

