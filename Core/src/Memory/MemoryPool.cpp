//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "MemoryPool.h"
#include "MemoryUtilities.h"

namespace Memory
{
    MemoryPool::MemoryPool(
        const UINT size)
        : m_valid(FALSE)
    {
        allocMem(size);
    }

    MemoryPool::~MemoryPool()
    {
        if (m_valid == TRUE)
        {
            freeMem();
        }
    }

    void* MemoryPool::requestMem(
        const UINT   size,
        const size_t alignment)
    {
        void* pResStart;

        m_pCurrent = MemoryUtility::MemoryAlignmentCeil(m_pCurrent, alignment);

        pResStart = m_pCurrent;

        assert((pResStart                             >= m_pStart) &&
               (static_cast<BYTE*>(m_pCurrent) + size <= m_pEnd));

        m_pCurrent += size;

        return pResStart;
    }

    void MemoryPool::clear()
    {
        m_pCurrent = m_pStart;
    }

    void MemoryPool::allocMem(
        const UINT size)
    {
        m_pStart   = static_cast<BYTE*>(malloc(size));
        m_pEnd     = m_pStart + size;
        m_pCurrent = m_pStart;

        m_valid = TRUE;
    }

    void MemoryPool::reallocMem(
        const UINT size)
    {
        freeMem();
        allocMem(size);
    }

    void MemoryPool::freeMem()
    {
        free(static_cast<void*>(m_pStart));

        m_pStart   = NULL;
        m_pEnd     = NULL;
        m_pCurrent = NULL;

        m_valid = FALSE;
    }
}
