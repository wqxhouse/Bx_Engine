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
	template<typename T, size_t SIZE>
	class QueueFixed
	{
	public:
		explicit QueueFixed(const BOOL isRing);

		~QueueFixed();

		void push(const T& val);

        T pop();

	private:
        UINT m_capacity;
        UINT m_size;

        BOOL m_isRing;

        T m_dataQueue[SIZE];

        UINT m_front;
        UINT m_back;
	};
}

#include "Queue.inl"