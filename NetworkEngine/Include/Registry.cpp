#include "Registry.h"

CRegistry::CRegistry()
{
	m_pRootKey = nullptr;
	m_bOpen = false;
}

CRegistry::~CRegistry()
{
}

bool CRegistry::Open(HKEY pRootKey, const TCHAR* pSubKey)
{
	if (m_bOpen || !pSubKey)
		return false;

	if (RegOpenKey(pRootKey, pSubKey, &m_pRootKey) != ERROR_SUCCESS)
		return false;

	m_bOpen = true;

	return true;
}

bool CRegistry::Close()
{
	if (!m_bOpen)
		return false;

	if (RegCloseKey(m_pRootKey) != ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::CreateKey(HKEY pRootKey, const TCHAR* pSubKey)
{
	if (m_bOpen || !pSubKey)
		return false;

	if (RegCreateKey(pRootKey, pSubKey, &m_pRootKey) != ERROR_SUCCESS)
		return false;

	m_bOpen = true;

	return true;
}

bool CRegistry::DeleteKey(HKEY pRootKey, const TCHAR* pSubKey)
{
	if (!pSubKey)
		return false;

	if (RegDeleteKey(pRootKey, pSubKey) != ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::SetValue(const TCHAR* pName, const TCHAR* pValue)
{
	if (!pName || !pValue)
		return false;

	if (!m_bOpen)
		return false;

	if (RegSetValueEx(m_pRootKey, pName, 0, REG_SZ, (BYTE*)pValue, lstrlen(pValue) * sizeof(TCHAR)) !=
		ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::SetValue(const TCHAR* pName, unsigned int dwValue)
{
	if (!pName)
		return false;

	if (!m_bOpen)
		return false;

	if (RegSetValueEx(m_pRootKey, pName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(dwValue)) !=
		ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::SetValueForMultiSz(const TCHAR* pName, const TCHAR* pValue, unsigned int dwLength)
{
	if (!pName || !pValue)
		return false;

	if (!m_bOpen)
		return false;

	if (RegSetValueEx(m_pRootKey, pName, 0, REG_MULTI_SZ, (BYTE*)pValue, dwLength) !=
		ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::GetValue(const TCHAR* pName, const TCHAR* pValue, unsigned int* pLength)
{
	if (!pName || !pValue || !pLength)
		return false;

	if (!m_bOpen)
		return false;

	DWORD	dwType = 0;

	if (RegQueryValueEx(m_pRootKey, pName, nullptr, &dwType, (BYTE*)pValue, (DWORD*)pLength) != ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::GetValue(const TCHAR* pName, const unsigned int* pValue)
{
	if (!pName || !pValue)
		return false;

	if (!m_bOpen)
		return false;

	DWORD	dwType = 0;
	DWORD	dwLength = sizeof(unsigned int);

	if (RegQueryValueEx(m_pRootKey, pName, nullptr, &dwType, (BYTE*)pValue, (DWORD*)&dwLength) != ERROR_SUCCESS)
		return false;

	return true;
}

bool CRegistry::DeleteValue(const TCHAR* pName)
{
	if (!pName)
		return false;

	if (!m_bOpen)
		return false;

	if (RegDeleteValue(m_pRootKey, pName) != ERROR_SUCCESS)
		return false;

	return true;
}
