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
            Trans* pTrans);

        ~ObjectBase();

        inline void ActiveObject()  { m_enable = TRUE;  }
        inline void DisableObject() { m_enable = FALSE; }

        inline const UINT   GetObjectId() const { return m_objectId;        }
        inline const Trans* GetTransPtr() const { return m_pTrans._Myptr(); }

    protected:
        std::unique_ptr<Trans> m_pTrans;

    private:
        UINT   m_objectId;
        BOOL   m_enable;
    };
}