//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Memory/BxMemory.h"

namespace Structure
{
	template<typename T>
	class Queue
	{
	public:
		Queue(
			const UINT capacity,
			const BOOL isRing);

		~Queue();

		void push(const T& val);

        T pop();

	private:
		Memory::LinearMemoryArena* m_pMemArena;

        UINT m_capacity;
        UINT m_size;

        BOOL m_isRing;

        T* m_pDataQueue;

        UINT m_front;
        UINT m_back;
	};
}

#include "Queue.inl"