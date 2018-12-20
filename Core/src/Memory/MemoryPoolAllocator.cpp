//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "MemoryPoolAllocator.h"
#include "MemoryUtilities.h"

namespace Memory
{
    MemoryPoolAllocator::MemoryPoolAllocator(
        MemoryPool*  pMemPool,
        const UINT   elementCount,
        const UINT   elementSize,
        const size_t offset,
        const size_t alignment)
        : m_pMemPool(pMemPool),
          m_elementCount(elementCount),
          m_elementSize(elementSize),
          m_offset(offset),
          m_alignment(alignment),
          m_elementAvaliableCount(elementCount)
    {
    }

    MemoryPoolAllocator::~MemoryPoolAllocator()
    {
        release();
    }

    void MemoryPoolAllocator::init(
        const size_t debugFrontSize,
        const size_t debugBackSize)
    {
        m_debugFrontSize = debugFrontSize;
        m_debugBackSize  = debugBackSize;

        // Align each element block
        m_elementSize = MemoryUtility::AlignmentCeil(
            static_cast<UINT>(m_elementSize + FREELIST_POINTER_SIZE + m_debugFrontSize + m_debugBackSize),
            m_alignment);

        m_size = m_elementCount * m_elementSize;

        // Align memory block
        m_pStart = static_cast<BYTE*>(m_pMemPool->requestMem(m_size, m_alignment));
        m_pEnd   = m_pStart + m_size;

        assert(m_pStart >= m_pMemPool->GetMemStartAddr() &&
               m_pEnd   <= m_pMemPool->GetMemEndAddr());

        m_size = static_cast<UINT>(m_pEnd - m_pStart);

        // Create freelist
        union IteratePtr
        {
            void*     ptr;
            BYTE*     ptrByte;
            FreeList* ptrFreeList;
        };

        IteratePtr prevPtr;
        prevPtr.ptrByte            = m_pStart;
        prevPtr.ptrFreeList->pPrev = NULL;
        prevPtr.ptrFreeList->pNext = ((m_elementCount > 1) ?
            reinterpret_cast<FreeList*>(prevPtr.ptrByte + m_elementSize) : NULL);

        m_freeListHead = prevPtr.ptrFreeList;

        IteratePtr curPtr = prevPtr;

        // Initialize free list
        for (UINT i = 0; i < m_elementCount - 1; ++i)
        {
            curPtr.ptrFreeList        = prevPtr.ptrFreeList->pNext;
            curPtr.ptrFreeList->pPrev = prevPtr.ptrFreeList;
            curPtr.ptrFreeList->pNext = reinterpret_cast<FreeList*>(curPtr.ptrByte + m_elementSize);

            prevPtr = curPtr;
        }

        curPtr.ptrFreeList->pNext = NULL;
    }

    void* MemoryPoolAllocator::alloc(
        const UINT   size,
        const size_t alignment,
        const size_t offset)
    {
        // Validate the alloc info
        assert(((static_cast<size_t>(size) + FREELIST_POINTER_SIZE) == m_elementSize) &&
               (alignment == m_alignment)                                             &&
               (offset    == m_debugFrontSize));

        void* resPtr = reinterpret_cast<BYTE*>(m_freeListHead) + FREELIST_POINTER_SIZE;

        assert(resPtr != NULL);

        FreeList* pNext = m_freeListHead->pNext;

        m_freeListHead->pPrev = NULL;
        m_freeListHead->pNext = NULL;

        m_freeListHead = pNext;

        return resPtr;
    }

    BOOL MemoryPoolAllocator::free(
        void* freeAddr)
    {
        FreeList* freeListHeaderPtr =
            reinterpret_cast<FreeList*>(static_cast<BYTE*>(freeAddr) - FREELIST_POINTER_SIZE);

        freeListHeaderPtr->pPrev = NULL;
        freeListHeaderPtr->pNext = m_freeListHead;

        if (m_freeListHead != NULL)
        {
            m_freeListHead->pPrev = freeListHeaderPtr;
        }

        m_freeListHead = freeListHeaderPtr;

        return BX_SUCCESS;
    }

    void MemoryPoolAllocator::clear()
    {
        union IteratePtr
        {
            void*     ptr;
            BYTE*     ptrByte;
            FreeList* ptrFreeList;
        };

        IteratePtr prevPtr;
        prevPtr.ptrByte = m_pStart;
        prevPtr.ptrFreeList->pPrev = NULL;
        prevPtr.ptrFreeList->pNext = ((m_elementCount > 1) ?
            reinterpret_cast<FreeList*>(prevPtr.ptrByte + m_elementSize) : NULL);

        m_freeListHead = prevPtr.ptrFreeList;

        IteratePtr curPtr = prevPtr;

        // Initialize free list
        for (UINT i = 0; i < m_elementCount - 1; ++i)
        {
            curPtr.ptrFreeList = prevPtr.ptrFreeList->pNext;
            curPtr.ptrFreeList->pPrev = prevPtr.ptrFreeList;
            curPtr.ptrFreeList->pNext = reinterpret_cast<FreeList*>(curPtr.ptrByte + m_elementSize);

            prevPtr = curPtr;
        }

        curPtr.ptrFreeList->pNext = NULL;
    }

    void MemoryPoolAllocator::release()
    {
        m_pStart       = NULL;
        m_pEnd         = NULL;
        m_freeListHead = NULL;

        if (m_pMemPool != NULL)
        {
            if (m_pMemPool->IsMemoryPoolValid())
            {
                m_pMemPool->freeMem();
            }

            m_pMemPool = NULL;
        }
    }

    void MemoryPoolAllocator::reBindPool(
        MemoryPool* pMemPool)
    {
        m_pMemPool = pMemPool;

        assert(pMemPool->IsMemoryPoolValid() == TRUE);
        
        init(m_debugFrontSize, m_debugBackSize);
    }
}
