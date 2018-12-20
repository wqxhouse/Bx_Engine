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
        explicit Allocator();
        explicit Allocator(MemoryRaw* pMem);

        ~Allocator();

        virtual void init(
            const size_t debugFrontSize = 0,
            const size_t debugBackSize  = 0) = 0;

        virtual void* alloc(const UINT   size,
                            const size_t alignment,
                            const size_t offset) = 0;

        virtual BOOL free(void* freeAddr) = 0;

        virtual void clear() = 0;

        virtual void release() = 0;

        INLINE UINT TotalMemory() const
        {
            return static_cast<UINT>(m_pEnd - m_pStart);
        }

        virtual INLINE UINT MemoryUsage() = 0;

        static const size_t DEFAULT_ALIGNMENT_SIZE = 4;

    protected:
        BYTE* m_pStart;
        BYTE* m_pEnd;
    };

    class LinearAllocator : public Allocator
    {
    public:
        explicit LinearAllocator(MemoryRaw* pMem);

        ~LinearAllocator();

        void init(
            const size_t debugFrontSize = 0,
            const size_t debugBackSize  = 0) {}

        void* alloc(
            const UINT   size,      ///< Allocate size
            const size_t alignment, ///< Alignment size
            const size_t offset);   ///< Allocate offset

        INLINE UINT MemoryUsage() const
        {
            return static_cast<UINT>(m_pCurrent - m_pStart);
        }

        // Not support linear memory allocation free
        INLINE BOOL free(void* freeAddr) const { return BX_FAIL; }

        INLINE void clear() { m_pCurrent = m_pStart; }

        virtual void release() {}

    private:
        BYTE* m_pCurrent;
    };

    /// @note Assump the maxmium allocation size is 4GB
    class StackAllocator : public Allocator
    {
    public:
        explicit StackAllocator(MemoryRaw* pMem);
        
        ~StackAllocator();

        void init(const size_t debugFrontSize = 0,
                  const size_t debugBackSize  = 0) {}

        void* alloc(
            const UINT   size,
            const size_t alignment,
            const size_t offset);

        BOOL free(void* freeAddr);

        void clear() { m_pCurrent = m_pStart; }

        virtual void release() {}

        INLINE UINT MemoryUsage() const
        {
            return static_cast<UINT>(m_pCurrent - m_pStart);
        }

    private:
        static const UINT ALLOC_OFFSET_HEADER_SIZE = sizeof(UINT);
        static const UINT ALLOC_SIZE_HEADER_SIZE   = sizeof(UINT);

        BYTE* m_pCurrent;
    };
}