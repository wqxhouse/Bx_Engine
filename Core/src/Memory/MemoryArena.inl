#include "MemoryArena.h"
//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

namespace Memory
{
    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    INLINE MemoryArena<Allocator, Thread, BoundChecking, MemoryTracker>::MemoryArena(
        const Allocator* pAllocator)
        : m_pAllocator(pAllocator)
    {
        m_memoryUsage = 0;
    }

    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    INLINE MemoryArena<Allocator, Thread, BoundChecking, MemoryTracker>::~MemoryArena()
    {
    }

    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    INLINE void* MemoryArena<Allocator, Thread, BoundChecking, MemoryTracker>::alloc(
        const UINT        size,
        const MemoryLogs& logs)
    {
        UINT allocSize = size + BoundChecking::FRONT_SIZE + BoundChecking::BACK_SIZE;

        assert(allocSize <= (m_pAllocator->TotalMemory() - m_memoryUsage));

        void* allocPtr = m_pAllocator->alloc(allocSize,
                                             Allocator::DEFAULT_ALIGNMENT_SIZE,
                                             BoundChecking::FRONT_SIZE);

        assert(allocPtr != NULL);

        BoundChecking::GuardFront(allocPtr);
        BoundChecking::GuardBack(allocPtr + BoundChecking::FROND_SIZE + size);

        m_memoryUsage += size;

        return result;
    }

    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    INLINE void* MemoryArena<Allocator, Thread, BoundChecking, MemoryTracker>::alloc(
        const UINT        size,
        const size_t      alignment,
        const MemoryLogs& logs)
    {
        UINT allocSize = size + BoundChecking::FRONT_SIZE + BoundChecking::BACK_SIZE;

        assert(allocSize <= (m_pAllocator->TotalMemory() - m_memoryUsage));

        void* allocPtr = m_pAllocator->alloc(allocSize,
                                             Allocator::DEFAULT_ALIGNMENT_SIZE,
                                             BoundChecking::FRONT_SIZE);

        assert(allocPtr != NULL);

        BoundChecking::GuardFront(allocPtr);
        BoundChecking::GuardBack(allocPtr + BoundChecking::FROND_SIZE + size);

        m_memoryUsage += size;

        return result;
    }

    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    INLINE BOOL MemoryArena<Allocator, Thread, BoundChecking, MemoryTracker>::free(
        void* freeAddr)
    {
        BOOL result = m_pAllocator->free(freeAddr);
        return result;
    }

    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    INLINE void MemoryArena<Allocator, Thread, BoundChecking, MemoryTracker>::clear()
    {
        m_pAllocator->clear();
    }
}