#pragma once

#include "Engine.h"

class CStream
{
public:
	CStream();
	~CStream();

private:
	char* m_pBuffer;
	unsigned int	m_iLength;

public:
	void SetBuffer(char* pBuffer)
	{
		m_pBuffer = pBuffer;
		m_iLength = 0;
	}

	unsigned int GetLength()
	{
		return m_iLength;
	}

public:
	template <typename T>
	bool Read(T* pData, int iSize)
	{
		memcpy(pData, m_pBuffer + m_iLength, iSize);
		m_iLength += iSize;

		return true;
	}

	template <typename T>
	bool Write(T* pData, int iSize)
	{
		memcpy(m_pBuffer + m_iLength, pData, iSize);
		m_iLength += iSize;

		return true;
	}
};

