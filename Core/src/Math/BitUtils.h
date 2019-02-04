//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2019
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Core/PCH.h"

namespace Math
{
    class BitUtils
    {
    public:
        static INLINE BOOL IsPowOfTwo(const size_t number)
        {
            return (((number - 1) & number) == 0);
        }
    };
}