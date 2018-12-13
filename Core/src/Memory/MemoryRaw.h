//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/PCH.h"

namespace Memory
{
    class MemoryRaw
    {
    public:
        MemoryRaw(void* pMemStart, void* pMemEnd);
        ~MemoryRaw();

        INLINE void* GetMemStartAddr() const { return m_pMemStart; }
        INLINE void* GetMemEndAddr()   const { return m_pMemEnd;   }

    private:
        void* m_pMemStart;
        void* m_pMemEnd;
    };

    template <size_t SIZE> // Linear memory size in byte
    class MemoryLinear : MemoryRaw
    {
    public:
        MemoryLinear()
            : MemoryRaw(m_linearMem, m_linearMem + SIZE),
            m_size(SIZE) {}

        ~MemoryLinear() {}

    private:
        BYTE         m_linearMem[SIZE];
        const size_t m_size;
    };

    template <size_t SIZE> // Stack memory size in byte
    class MemoryStack : MemoryRaw
    {
    public:
        MemoryStack()
            : MemoryRaw(m_stackMem, m_stackMem + SIZE),
              m_size(SIZE) {}

        ~MemoryStack() {}

    private:
        BYTE         m_stackMem[SIZE];
        const size_t m_size;
    };

    // TODO: Heap memory
}