#pragma once

#include "ThreadSync.h"

template <typename T, int ALLOCK_SIZE = 50>
class CMemoryPool	:
	public CThreadSync<CMemoryPool>
{
protected: // 소멸자
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
		// 타입을 바꿀려는데 포인터를 이중포인터로 바꾼다라
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

		UCHAR** pCurrent = reinterpret_cast<UCHAR**>(m_pArray); // 이중포인터로 변환
		UCHAR* pNext = m_pArray;

		for (int i = 0; i < ALLOCK_SIZE -[ 1; ++i)
		{
			pNext += sizeof(T); // 크기더해주고 즉, array에서 T크기만큼 이동
			*pCurrent = pNext; // 해당 위치 가리키게하고 
			pCurrent = reinterpret_cast<UCHAR**>(pNext);
			// current에 이동한 array위치 넣어주고
		}

		*pCurrent = 0;
	}
};


template <typename T, int ALLOCK_SIZE>
UCHAR* CMemoryPool<T, ALLOCK_SIZE>::m_pArray;

/*
	Memory Pool의 전략

1. 미리 일정 크기의 Memory를 할당 받습니다.

2. Memory 할당이 필요한 객체에 대해서 시스템콜을 부르지 않고 미리 할

당받은 Memory 구역 중 일부를 할당합니다.

3. 해당 객체의 사용이 끝나면 Memory Pool로 해당 메모리 구역을 반환 합

니다.

Memory Pool의 장점

1. 여러 작은 객체들이 new / delete를 해야하는 상황이라고 하겠습니다.

이 객체들에 대해서 매번 new / delete를 해주면 시스템 콜이 발생합니다.

이 때 생기는 커널, 유저 모드의 전환은 상당한 비용을 야기합니다. 미리

Memory를 할당해 놓은 Pool을 사용하는 경우 이 비용을 아낄 수 있습니다.

2. 또 잦은 객체의 할당과 해제는 메모리 파편화를 불러올 수 있습니다. 이

때 미리 잡아 놓은 Memory Pool을 이용해 파편화를 최소화 할 수 있습니

다.


그렇다면 오브젝트풀은?
장점) 메모리를 신경쓰지 않고 마음껏 객체를 생성, 삭제할 수 있다.
단점) 객체 아무것도 사용하지 않아도 메모리를 차지하고 있는 상태가 됨.
객체 풀 클래스에 들어가는 객체는 현재 자신이 사용중인지 아닌지 알 수 있는 방법을 제공해야 한다.

1. 풀은 초기화될 때 사용할 객체들을 미리 생성하여 배열에 넣어두고 사용 안함 상태로 초기화 한다.
2. 새로운 객체가 필요하면 풀에 요청한다.
3. 풀은 사용 가능한 객체를 찾아 사용 중으로 초기화 한 뒤 리턴한다.
4. 객체를 더 이상 사용하지 않는다면 사용 안 함 상태로 되돌린다.

오브젝트풀 패턴을 언제 쓸 것인가?
●객체를 빈번하게 생성/삭제 해야할 때
●객체들의 크기가 비슷할 때
●객체를 힙에 생성하기엔 느리거나 메모리 단편화가 우려될 때
●DB 연결이나 네트워크 연결같이 객체 생성시 비용이 비쌀 경우 미리 생성해두고 재사용할 필요가 있을 때

Object Pool 사용시 주의사항

1. Object의 개수를 어떻게 조절할 것인가?
Object Pool을 구성하면 일단은 해당 객체에 대해서 제한된 개수만 한 번
에 사용할 수 있게 됩니다. 그렇다면 객체가 제한된 개수 이상으로 필요한
때에는 어떤 조치를 취해야 할까요?



   1.1 애당초 `절대` 그런일이 일어나지 않도록 Pool Size를 조절한다
게임 플레이 상에 필요한 아이템이나 적 몬스터 등 꼭 필요한 객체에 알맞
은 방식입니다. 이 방식의 단점은 몇몇 특이 사항 때문에 너무 큰 Pool
Size를 유지해야 될 수 있다는 점입니다. 이를 해결하기 위해 시나리오에
따라 Pool Size를 조절하기도 합니다.



   1.2 객체를 생성하지 않는다
파티클 시스템에 적절할 수 있습니다. 모든 파티클이 사용중이라면 이미
화면은 파티클로 차있을 것이고 몇 개의 파티클을 더 추가하지 않아도 눈
치채지 못 할 것입니다.

   1.3 존재하는 객체를 할당 해제 한다
현재 플레이 되고 있는 Sound를 그만 끄고 새로운 Sound를 시작하고 싶을
수 있습니다. 가장 최선의 방식은 이미 나오고 있는 Sound중에 가장 조용
한 Sound를 할당 해제하고 새로운 Sound를 플레이 하는 것입니다.
  
  1.4 Pool의 크기를 늘린다


2. 재사용 객체는 스스로 지워지지 않는다
Object Pool에서 관리하는 객체를 앞으로 전혀 쓸일이 없을 때 꼭 지워줘
야 합니다.

3. 각 객체의 메모리 크기는 고정되어 있다.
일반적으로 ObjectPool에 같은 타입을 넣는 것이 상례지만,
만약 하나의 Object Pool에 다른 타입의 객체를 넣는 다면 어떻게 될까요? 
추가 멤버 변수를 갖는 subClass 같은것 말입니다.
이 때는 항상 ObjectPool이 최대 크기를 할당할 수 있는지 확인해야 합니다.
너무 작은 크기가 들어와도 문제입니다. 메모리가 낭비되기 때문입니다.

출처: https://itability.tistory.com/40 [aBiLiTy BLoG]
*/