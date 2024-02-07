#pragma once

#include "ThreadSync.h"

template <typename T, int ALLOCK_SIZE = 50>
class CMemoryPool	:
	public CThreadSync<CMemoryPool>
{
protected: // �Ҹ���
	~CMemoryPool()
	{
	}

private:
	static UCHAR* m_pArray;

public:
	static void* operator new (std::size_t iSize)
	{
		CThreadSync	sync;

		assert(sizeof(T) == iSize);
		assert(sizeof(T) >= sizeof(UCHAR*));d

		if (!m_pArray)
			AllocArray();

		UCHAR* pReturnPointer = m_pArray;
		m_pArray = *reinterpret_cast<UCHAR**>(pReturnPointer); 
		// Ÿ���� �ٲܷ��µ� �����͸� ���������ͷ� �ٲ۴ٶ�
		return pReturnPointer;
	}

	static void operator delete (void* pPointer)
	{
		CThreadSync	sync;

		*reinterpret_cast<UCHAR**>(pPointer) = m_pArray;
		m_pArray = static_cast<UCHAR*>(pPointer);
	}
	      
private:
	static void AllocArray()
	{
		m_pArray = new UCHAR[sizeof(T) * ALLOCK_SIZE];

		UCHAR** pCurrent = reinterpret_cast<UCHAR**>(m_pArray); // ���������ͷ� ��ȯ
		UCHAR* pNext = m_pArray;

		for (int i = 0; i < ALLOCK_SIZE -[ 1; ++i)
		{
			pNext += sizeof(T); // ũ������ְ� ��, array���� Tũ�⸸ŭ �̵�
			*pCurrent = pNext; // �ش� ��ġ ����Ű���ϰ� 
			pCurrent = reinterpret_cast<UCHAR**>(pNext);
			// current�� �̵��� array��ġ �־��ְ�
		}

		*pCurrent = 0;
	}
};


template <typename T, int ALLOCK_SIZE>
UCHAR* CMemoryPool<T, ALLOCK_SIZE>::m_pArray;

/*
	Memory Pool�� ����

1. �̸� ���� ũ���� Memory�� �Ҵ� �޽��ϴ�.

2. Memory �Ҵ��� �ʿ��� ��ü�� ���ؼ� �ý������� �θ��� �ʰ� �̸� ��

����� Memory ���� �� �Ϻθ� �Ҵ��մϴ�.

3. �ش� ��ü�� ����� ������ Memory Pool�� �ش� �޸� ������ ��ȯ ��

�ϴ�.

Memory Pool�� ����

1. ���� ���� ��ü���� new / delete�� �ؾ��ϴ� ��Ȳ�̶�� �ϰڽ��ϴ�.

�� ��ü�鿡 ���ؼ� �Ź� new / delete�� ���ָ� �ý��� ���� �߻��մϴ�.

�� �� ����� Ŀ��, ���� ����� ��ȯ�� ����� ����� �߱��մϴ�. �̸�

Memory�� �Ҵ��� ���� Pool�� ����ϴ� ��� �� ����� �Ƴ� �� �ֽ��ϴ�.

2. �� ���� ��ü�� �Ҵ�� ������ �޸� ����ȭ�� �ҷ��� �� �ֽ��ϴ�. ��

�� �̸� ��� ���� Memory Pool�� �̿��� ����ȭ�� �ּ�ȭ �� �� �ֽ���

��.


�׷��ٸ� ������ƮǮ��?
����) �޸𸮸� �Ű澲�� �ʰ� ������ ��ü�� ����, ������ �� �ִ�.
����) ��ü �ƹ��͵� ������� �ʾƵ� �޸𸮸� �����ϰ� �ִ� ���°� ��.
��ü Ǯ Ŭ������ ���� ��ü�� ���� �ڽ��� ��������� �ƴ��� �� �� �ִ� ����� �����ؾ� �Ѵ�.

1. Ǯ�� �ʱ�ȭ�� �� ����� ��ü���� �̸� �����Ͽ� �迭�� �־�ΰ� ��� ���� ���·� �ʱ�ȭ �Ѵ�.
2. ���ο� ��ü�� �ʿ��ϸ� Ǯ�� ��û�Ѵ�.
3. Ǯ�� ��� ������ ��ü�� ã�� ��� ������ �ʱ�ȭ �� �� �����Ѵ�.
4. ��ü�� �� �̻� ������� �ʴ´ٸ� ��� �� �� ���·� �ǵ�����.

������ƮǮ ������ ���� �� ���ΰ�?
�ܰ�ü�� ����ϰ� ����/���� �ؾ��� ��
�ܰ�ü���� ũ�Ⱑ ����� ��
�ܰ�ü�� ���� �����ϱ⿣ �����ų� �޸� ����ȭ�� ����� ��
��DB �����̳� ��Ʈ��ũ ���ᰰ�� ��ü ������ ����� ��� ��� �̸� �����صΰ� ������ �ʿ䰡 ���� ��

Object Pool ���� ���ǻ���

1. Object�� ������ ��� ������ ���ΰ�?
Object Pool�� �����ϸ� �ϴ��� �ش� ��ü�� ���ؼ� ���ѵ� ������ �� ��
�� ����� �� �ְ� �˴ϴ�. �׷��ٸ� ��ü�� ���ѵ� ���� �̻����� �ʿ���
������ � ��ġ�� ���ؾ� �ұ��?



   1.1 �ִ��� `����` �׷����� �Ͼ�� �ʵ��� Pool Size�� �����Ѵ�
���� �÷��� �� �ʿ��� �������̳� �� ���� �� �� �ʿ��� ��ü�� �˸�
�� ����Դϴ�. �� ����� ������ ��� Ư�� ���� ������ �ʹ� ū Pool
Size�� �����ؾ� �� �� �ִٴ� ���Դϴ�. �̸� �ذ��ϱ� ���� �ó�������
���� Pool Size�� �����ϱ⵵ �մϴ�.



   1.2 ��ü�� �������� �ʴ´�
��ƼŬ �ý��ۿ� ������ �� �ֽ��ϴ�. ��� ��ƼŬ�� ������̶�� �̹�
ȭ���� ��ƼŬ�� ������ ���̰� �� ���� ��ƼŬ�� �� �߰����� �ʾƵ� ��
ġä�� �� �� ���Դϴ�.

   1.3 �����ϴ� ��ü�� �Ҵ� ���� �Ѵ�
���� �÷��� �ǰ� �ִ� Sound�� �׸� ���� ���ο� Sound�� �����ϰ� ����
�� �ֽ��ϴ�. ���� �ּ��� ����� �̹� ������ �ִ� Sound�߿� ���� ����
�� Sound�� �Ҵ� �����ϰ� ���ο� Sound�� �÷��� �ϴ� ���Դϴ�.
  
  1.4 Pool�� ũ�⸦ �ø���


2. ���� ��ü�� ������ �������� �ʴ´�
Object Pool���� �����ϴ� ��ü�� ������ ���� ������ ���� �� �� ������
�� �մϴ�.

3. �� ��ü�� �޸� ũ��� �����Ǿ� �ִ�.
�Ϲ������� ObjectPool�� ���� Ÿ���� �ִ� ���� �������,
���� �ϳ��� Object Pool�� �ٸ� Ÿ���� ��ü�� �ִ� �ٸ� ��� �ɱ��? 
�߰� ��� ������ ���� subClass ������ ���Դϴ�.
�� ���� �׻� ObjectPool�� �ִ� ũ�⸦ �Ҵ��� �� �ִ��� Ȯ���ؾ� �մϴ�.
�ʹ� ���� ũ�Ⱑ ���͵� �����Դϴ�. �޸𸮰� ����Ǳ� �����Դϴ�.

��ó: https://itability.tistory.com/40 [aBiLiTy BLoG]
*/