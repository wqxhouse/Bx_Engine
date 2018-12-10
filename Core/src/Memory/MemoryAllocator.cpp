//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "MemoryUtilities.h"
#include "MemoryAllocator.h"

namespace Memory
{
    /// Base Allocator
    Allocator::Allocator(MemoryRaw* pMem)
        : m_pStart(static_cast<BYTE*>(pMem->GetMemStartAddr())),
          m_pEnd(static_cast<BYTE*>(pMem->GetMemEndAddr())),
          m_pCurrent(m_pStart)
    {
    }

    Allocator::~Allocator()
    {
    }

    /// Linear Allocator
    LinearAllocator::LinearAllocator(
        MemoryRaw* pMem)
        : Allocator(pMem)
    {

    }

    LinearAllocator::~LinearAllocator()
    {
    }
    
    void* LinearAllocator::alloc(
        const UINT   size,      ///< Allocate size
        const size_t alignment, ///< Alignment size
        const size_t offset)    ///< Allocate offset
    {
        m_pCurrent =
            MemoryUtility::MemoryAlignmentCeil(m_pCurrent + offset, alignment) - offset;

        void* resPtr = m_pCurrent;
        m_pCurrent += size;

        assert(m_pCurrent < m_pEnd);

        return resPtr;
    }
    
    /// Stack Allocator
    StackAllocator::StackAllocator(MemoryRaw * pMem)
        : Allocator(pMem)
    {
    }

    StackAllocator::~StackAllocator()
    {
    }

    void* StackAllocator::alloc(
        const size_t size,
        const size_t alignment,
        const size_t offset)
    {
        NotImplemented();

        return NULL;
    }

    BOOL StackAllocator::free(
        void* freeAddr)
    {
        BOOL result = BX_SUCCESS;

        NotImplemented();

        return result;
    }

    void StackAllocator::clear()
    {
        NotImplemented();
    }
}