//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/PCH.h"

namespace Memory
{
    struct MemoryBoundChecker
    {
        static INLINE void GuardFront(void* const frontAddr)
        {
            *(static_cast<UINT*>(frontAddr)) = FRONT_MAGIC;
        }

        static INLINE void GuardBack(void* const backAddr)
        {
            *(static_cast<UINT*>(backAddr)) = BACK_MAGIC;
        }

        static INLINE BOOL ValidateFront(void* const frontAddr)
        {
            BOOL result =
                ((*(static_cast<UINT*>(frontAddr)) == FRONT_MAGIC) ? BX_SUCCESS : BX_FAIL);

            assert(result == BX_SUCCESS);

            return result;
        }

        static INLINE BOOL ValidateBack(void* const backAddr)
        {
            BOOL result =
                ((*(static_cast<UINT*>(backAddr)) == BACK_MAGIC) ? BX_SUCCESS : BX_FAIL);

            assert(result == BX_SUCCESS);

            return result;
        }

        static const size_t FRONT_SIZE = 4;
        static const size_t BACK_SIZE  = 4;

        static const UINT FRONT_MAGIC = 0xBEABCDEF;
        static const UINT BACK_MAGIC  = 0xDEADEDED;
    };

    struct NoMemoryBoundChecker
    {
        static const size_t FRONT_SIZE = 0;
        static const size_t BACK_SIZE  = 0;

        // No Bound check, just left empty inline functions
        static INLINE void GuardFront(void* const frontAddr)    {}
        static INLINE void GuardBack(void* const backAddr)      {}
        static INLINE BOOL ValidateFront(void* const frontAddr) {}
        static INLINE BOOL ValidateBack(void* const backAddr)   {}
    };

    struct ThreadGuard
    {
        // TODO
    };

    struct NoThreadGuard
    {
        // TODO
    };

    struct MemoryTracker
    {
        // TODO
    };

    struct NoMemoryTracker
    {
        // TODO
    };

    struct MemoryLogs
    {
        const char* fileName;
        const int   lineNum;
    };

    class MemoryUtility
    {
    public:
        template <typename T>
        static INLINE T AlignmentCeil(
            const T&     val,
            const size_t alignmentNum)
        {
            size_t valInt = static_cast<size_t>(val);
            T res         = static_cast<T>(valInt);

            size_t mod    = valInt % alignmentNum;

            if (mod != 0)
            {
                res = static_cast<T>(valInt + mod);
            }

            return res;
        }

        template <typename T>
        static INLINE T* MemoryAlignmentCeil(
            T*           pAddr,
            const size_t alignmentNum)
        {
            size_t mod = (*(reinterpret_cast<const size_t*>(pAddr)) % alignmentNum);

            if (mod != 0)
            {
                pAddr += (alignmentNum - mod);
            }

            return static_cast<BYTE*>(pAddr);
        }

        template <typename T>
        static INLINE T* MemoryAlignmentFloor(
            T*           pAddr, 
            const size_t alignmentNum)
        {
            size_t mod = (*(reinterpret_cast<const size_t*>(pAddr)) % alignmentNum);

            if (mod != 0)
            {
                pAddr -= mod;
            }

            return static_cast<BYTE*>(pAddr);
        }
    };
}