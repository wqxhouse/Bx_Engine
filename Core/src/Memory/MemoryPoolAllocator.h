//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "MemoryAllocator.h"
#include "MemoryPool.h"

namespace Memory
{
    class MemoryPoolAllocator : public Allocator
    {
    public:
        explicit MemoryPoolAllocator(
            MemoryPool*  pMemPool,
            const UINT   elementCount,
            const UINT   elementSize,
            const size_t offset,
            const size_t alignment);

        ~MemoryPoolAllocator();

        void init(const size_t debugFrontSize = 0,
                  const size_t debugBackSize  = 0);

        void* alloc(const UINT   size,
                    const size_t alignment,
                    const size_t offset);

        BOOL free(void* freeAddr);

        INLINE UINT MemoryUsage() { return (m_size - m_elementAvaliableCount * m_elementSize); }

        void clear();

        void release();

        void reBindPool(
            MemoryPool* pMemPool);

    private:
        struct FreeList
        {
            FreeList* pPrev;
            FreeList* pNext;
        };

        FreeList* m_freeListHead;

        MemoryPool* m_pMemPool;

        UINT m_size;
        UINT m_elementSize;

        const UINT   m_elementCount;
        const size_t m_offset;
        const size_t m_alignment;

        UINT m_elementAvaliableCount;

        size_t m_debugFrontSize;
        size_t m_debugBackSize;

        static const size_t FREELIST_POINTER_SIZE = sizeof(FreeList);
    };
}