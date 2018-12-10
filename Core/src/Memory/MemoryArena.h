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
#include "MemoryUtilities.h"

namespace Memory
{
    template<class Allocator, class Thread, class BoundChecking, class MemoryTracker>
    class MemoryArena
    {
    public:
        MemoryArena(
            MemoryRaw*       pMem,
            const Allocator* pAllocator);

        ~MemoryArena();

        void* alloc(
            const UINT size,
            const MemoryLogs& logs);

        void* alloc(
            const UINT        size,
            const size_t      alignment,
            const MemoryLogs& logs);

        BOOL free(void* freeAddr);

        void clear();

    private:
        MemoryRaw*           m_pMem;
        const Allocator*     m_pAllocator;

        size_t m_memoryUsage;
    };
}

#include "MemoryArena.inl"