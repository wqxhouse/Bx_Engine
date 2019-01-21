//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../Math/Transform/Trans.h"

namespace Object
{
    class ObjectBase
    {
    public:
        explicit ObjectBase(
            const UINT objectId,
            Trans*     pTrans);

        ~ObjectBase();

        INLINE void ActiveObject()  { m_enable = TRUE;  }
        INLINE void DisableObject() { m_enable = FALSE; }

        INLINE const UINT   GetObjectId() const { return m_objectId; }
        INLINE const Trans* GetTransPtr() const { return m_pTrans;   }
        INLINE const BOOL   IsEnable()    const { return m_enable;   }

    protected:
        Trans* m_pTrans;

    private:
        UINT m_objectId;
        BOOL m_enable;
    };
}