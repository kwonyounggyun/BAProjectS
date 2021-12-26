#pragma once
#include <Windows.h>
#include "BACriticalSection.h"

//메모리 풀은 클래스 단위다.
template<typename T, int POOL_SIZE = 100, bool FIX_SIZE = false>
class BAMemoryPool
{
private:
	static unsigned char* m_free_ptr;
	static bool m_fix;
	static BACS cs;

public:
	static void* operator new(size_t size)
	{
		BASmartCS(&cs);
		if (m_free_ptr == nullptr && m_fix == false)
			Alloc();
		else if (m_free_ptr == nullptr && m_fix == true)
			return nullptr;

		unsigned char* return_ptr = m_free_ptr;
		m_free_ptr = *reinterpret_cast<unsigned char**>(m_free_ptr);
		return return_ptr;
	}

	static void operator delete(void* ptr) noexcept
	{
		BASmartCS(&cs);
		memset(ptr, 0x00, sizeof(T));
		*reinterpret_cast<unsigned char**>(ptr) = m_free_ptr;
		m_free_ptr = reinterpret_cast<unsigned char*>(ptr);
	}

private:
	static void Alloc()
	{
		if (m_fix == false && FIX_SIZE == true)
			m_fix = true;

		unsigned char* alloc_memory = new unsigned char[sizeof(T) * POOL_SIZE];

		T* next = reinterpret_cast<T*>(alloc_memory);
		for (int i = 0; i < POOL_SIZE - 1; i++)
		{
			*(reinterpret_cast<T**>(next)) = next + 1;
			next++;
		}
		*(reinterpret_cast<T**>(next)) = nullptr;

		m_free_ptr = alloc_memory;
	}
};

template <typename T, int POOL_SIZE, bool FIX_SIZE>
unsigned char* BAMemoryPool<T, POOL_SIZE, FIX_SIZE>::m_free_ptr = nullptr;

template <typename T, int POOL_SIZE, bool FIX_SIZE>
bool BAMemoryPool<T, POOL_SIZE, FIX_SIZE>::m_fix = false;

template <typename T, int POOL_SIZE, bool FIX_SIZE>
BACS BAMemoryPool<T, POOL_SIZE, FIX_SIZE>::cs;