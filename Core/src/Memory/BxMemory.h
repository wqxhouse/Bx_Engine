//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "MemoryAllocator.h"
#include "MemoryArena.h"

namespace Memory
{
#if _DEBUG
    typedef MemoryArena<LinearAllocator, ThreadGuard, MemoryBoundChecker, MemoryTracker> LinearMemoryArena;
    typedef MemoryArena<StackAllocator,  ThreadGuard, MemoryBoundChecker, MemoryTracker> StackMemoryArena;
#else
    typedef MemoryArena<LinearAllocator, NoThreadGuard, NoMemoryBoundChecker, NoMemoryTracker> LinearMemoryArena;
    typedef MemoryArena<StackAllocator,  NoThreadGuard, NoMemoryBoundChecker, NoMemoryTracker> StackMemoryArena;
#endif
}