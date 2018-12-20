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
    Allocator::Allocator()
    {
    }

    Allocator::Allocator(MemoryRaw* pMem)
        : m_pStart(static_cast<BYTE*>(pMem->GetMemStartAddr())),
          m_pEnd(static_cast<BYTE*>(pMem->GetMemEndAddr()))
    {
    }

    Allocator::~Allocator()
    {
    }

    /// Linear Allocator
    LinearAllocator::LinearAllocator(
        MemoryRaw* pMem)
        : Allocator(pMem),
          m_pCurrent(m_pStart)
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

        void* resPtr = static_cast<void*>(m_pCurrent);
        m_pCurrent += size;

        assert(m_pCurrent <= m_pEnd);

        return resPtr;
    }
    
    /// Stack Allocator
    StackAllocator::StackAllocator(MemoryRaw* pMem)
        : Allocator(pMem),
          m_pCurrent(m_pStart)
    {
    }

    StackAllocator::~StackAllocator()
    {
    }

    void* StackAllocator::alloc(
        const UINT   size,
        const size_t alignment,
        const size_t offset)
    {
        // Added the header (allocation offset) size
        UINT allocSize = size + ALLOC_OFFSET_HEADER_SIZE + ALLOC_SIZE_HEADER_SIZE;

        m_pCurrent =
            MemoryUtility::MemoryAlignmentCeil(m_pCurrent + offset, alignment) - offset;

        union
        {
            void* pRes;
            BYTE* pByte;
            UINT* pUint;
        };

        pByte  = m_pCurrent;
        *pUint = static_cast<UINT>(m_pCurrent - m_pStart); // Write the offset value into the header of
                                                           // each allocation

        pByte += ALLOC_OFFSET_HEADER_SIZE;
        *pUint = size;  // Write the allocation size into the header of each allocation

        pByte += ALLOC_SIZE_HEADER_SIZE;

        m_pCurrent += allocSize; // Move the current pointer to the end of data region

        assert(m_pCurrent <= m_pEnd);

        return pRes;
    }

    BOOL StackAllocator::free(
        void* freeAddr)
    {
        BOOL result = BX_SUCCESS;

        union
        {
            BYTE* pByte;
            UINT* pUint;
        };

        pByte = static_cast<BYTE*>(freeAddr) - ALLOC_SIZE_HEADER_SIZE - ALLOC_OFFSET_HEADER_SIZE;
        UINT offset = *pUint;

        pByte += ALLOC_SIZE_HEADER_SIZE;
        UINT size = *pUint;

        assert(pByte + size == m_pCurrent); // The free should follow LIFO rule
        
        m_pCurrent = m_pStart + offset;

        return result;
    }
}