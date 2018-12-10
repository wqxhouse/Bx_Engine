//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "MemoryRaw.h"

namespace Memory
{
    MemoryRaw::MemoryRaw(void* pMemStart, void* pMemEnd)
        : m_pMemStart(pMemStart),
          m_pMemEnd(pMemEnd)
    {
    }

    MemoryRaw::~MemoryRaw()
    {
    }
}
