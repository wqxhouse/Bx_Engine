//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "MemoryPoolAllocator.h"
#include "MemoryArena.h"

#define BX_NEW(T, arena)      new(arena.alloc(sizeof(T), {__FILE__, __LINE__})) T
#define BX_DELETE(ptr, arena) (Memory::BxMemory::Delete(&ptr, arena))
#define BX_RELEASE(arena)     (arena.release())

namespace Memory
{
#if _DEBUG
    typedef MemoryArena<LinearAllocator, ThreadGuard, MemoryBoundChecker, MemoryTracker>     LinearMemoryArena;
    typedef MemoryArena<StackAllocator,  ThreadGuard, MemoryBoundChecker, MemoryTracker>     StackMemoryArena;
    typedef MemoryArena<MemoryPoolAllocator, ThreadGuard, MemoryBoundChecker, MemoryTracker> MemoryPoolArena;
#else
    typedef MemoryArena<LinearAllocator, NoThreadGuard, NoMemoryBoundChecker, NoMemoryTracker>     LinearMemoryArena;
    typedef MemoryArena<StackAllocator,  NoThreadGuard, NoMemoryBoundChecker, NoMemoryTracker>     StackMemoryArena;
    typedef MemoryArena<MemoryPoolAllocator, NoThreadGuard, NoMemoryBoundChecker, NoMemoryTracker> MemoryPoolArena;
#endif

    class BxMemory
    {
    public:
        template<typename T, typename MemoryArena>
        static void Delete(T** ptr, MemoryArena* arena)
        {
            (*ptr)->~T();
            arena->free(*ptr);

            *ptr = NULL;
        }
    };
}