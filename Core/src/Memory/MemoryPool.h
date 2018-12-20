//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "MemoryRaw.h"

namespace Memory
{
    class MemoryPool
    {
    public:
        explicit MemoryPool(
            const UINT size);

        ~MemoryPool();

        void* requestMem(
            const UINT   size,
            const size_t alignment);

        void clear();

        void allocMem(
            const UINT size);

        void reallocMem(
            const UINT size);

        void freeMem();

        INLINE BOOL  IsMemoryPoolValid() const { return m_valid;  }
        INLINE void* GetMemStartAddr()   const { return m_pStart; }
        INLINE void* GetMemEndAddr()     const { return m_pEnd;   }

    private:
        BYTE* m_pStart;
        BYTE* m_pEnd;
        BYTE* m_pCurrent;

    private:
        BOOL m_valid; // If the memory pool is valid(allocated)
    };
}