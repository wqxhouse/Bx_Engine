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
    class Allocator
    {
    public:
        Allocator(MemoryRaw* pMem);

        ~Allocator();

        virtual void* alloc(const size_t size,
                           const size_t alignment,
                           const size_t offset) = 0;

        virtual BOOL free(void* freeAddr) = 0;

        virtual void clear() = 0;

        static const size_t DEFAULT_ALIGNMENT_SIZE = 4;

    protected:
        BYTE* m_pStart;
        BYTE* m_pEnd;
        BYTE* m_pCurrent;
    };

    class LinearAllocator : public Allocator
    {
    public:
        LinearAllocator(MemoryRaw* pMem);

        ~LinearAllocator();

        void* alloc(
            const UINT   size,      ///< Allocate size
            const size_t alignment, ///< Alignment size
            const size_t offset);   ///< Allocate offset

        // Not support linear memory allocation free
        INLINE BOOL free(void* freeAddr) const { return BX_FAIL; }

        INLINE void clear() { m_pCurrent = m_pStart; }
    };

    class StackAllocator : public Allocator
    {
    public:
        StackAllocator(MemoryRaw* pMem);
        
        ~StackAllocator();

        void* alloc(
            const size_t size,
            const size_t alignment,
            const size_t offset);

        BOOL free(void* freeAddr);

        void clear();
    };
}