#include "IniFile.h"

CIniFile::CIniFile()
{
	memset(m_strFileName, 0, sizeof(TCHAR) * MAX_PATH);
}

CIniFile::~CIniFile()
{
}

bool CIniFile::Open(const TCHAR* pFileName)
{
	if (!pFileName)
		return false;

	lstrcpy(m_strFileName, pFileName);

	return true;
}

bool CIniFile::Close()
{
	return true;
}

bool CIniFile::SetValue(const TCHAR* pKeyName, const TCHAR* pValueName, const TCHAR* pValue)
{
	if (!pKeyName || !pValueName || !pValue)
		return false;

	WritePrivateProfileString(pKeyName, pValueName, pValue, m_strFileName);

	return true;
}

bool CIniFile::SetValue(const TCHAR* pKeyName, const TCHAR* pValueName, unsigned int iValue)
{
	if (!pKeyName || !pValueName)
		return false;

	TCHAR	strValue[16] = {};
	_itow_s(iValue, strValue, 10);

	WritePrivateProfileString(pKeyName, pValueName, strValue, m_strFileName);

	return true;
}

bool CIniFile::SetValue(const TCHAR* pKeyName, const TCHAR* pValueName, float fValue)
{
	if (!pKeyName || !pValueName)
		return false;

	TCHAR	strValue[16] = {};
	_stprintf_s(strValue, TEXT("%f"), fValue);

	WritePrivateProfileString(pKeyName, pValueName, strValue, m_strFileName);

	return true;
}

bool CIniFile::GetValue(const TCHAR* pKeyName, const TCHAR* pValueName, TCHAR* pValue, 
	unsigned int* pLength)
{
	if (!pKeyName || !pValueName || !pValue || !pLength)
		return false;

	*pLength = GetPrivateProfileString(pKeyName, pValueName, TEXT(""), pValue,
		*pLength, m_strFileName);

	return true;
}

bool CIniFile::GetValue(const TCHAR* pKeyName, const TCHAR* pValueName, unsigned int* pValue)
{
	if (!pKeyName || !pValueName || !pValue)
		return false;

	*pValue = GetPrivateProfileInt(pKeyName, pValueName, 0, m_strFileName);

	return true;
}

bool CIniFile::GetValue(const TCHAR* pKeyName, const TCHAR* pValueName, float* pValue)
{
	if (!pKeyName || !pValueName || !pValue)
		return false;

	TCHAR	strValue[16] = {};

	GetPrivateProfileString(pKeyName, pValueName, TEXT(""), strValue, 16, m_strFileName);

	*pValue = (float)_tstof(strValue);

	return true;
}
