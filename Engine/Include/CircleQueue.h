#pragma once

#include "ThreadSync.h"

template <typename T, int NUMBER = 100>
class CCircleQueue	:
	public CThreadSync<CCircleQueue<T, NUMBER>>
{
public:
	CCircleQueue()
	{
		m_iHead = 0;
		m_iTail = 0;
		m_iSize = 0;
	}

	~CCircleQueue()
	{
	}

private:
	T		m_Array[NUMBER + 1];
	int		m_iHead;
	int		m_iTail;
	int		m_iSize;

public:
	void push(const T& data)
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		int	ALTER = (m_iTail + 1) % (NUMBER + 1);

		if (ALTER == m_iHead)
			return;

		m_Array[ALTER] = data;

		m_iTail = ALTER;

		++m_iSize;
	}

	T front()	const
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		if (m_iHead == m_iTail)
		{
			assert(false);
		}

		int	iHead = (m_iHead + 1) % (NUMBER + 1);

		return m_Array[iHead];
	}

	void pop()
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		if (m_iHead == m_iTail)
		{
			assert(false);
		}

		m_iHead = (m_iHead + 1) % (NUMBER + 1);

		--m_iSize;
	}

	bool empty()	const
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		return m_iHead == m_iTail;
	}

	bool full()	const
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		int	iTail = (m_iTail + 1) % (NUMBER + 1);

		return iTail == m_iHead;
	}

	void clear()
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		m_iHead = 0;
		m_iTail = 0;
		m_iSize = 0;
	}

	int size()	const
	{
		CThreadSync<CCircleQueue<T, NUMBER>>	sync;

		return m_iSize;
	}
};
