//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "ObjectBase.h"

namespace Object
{
    ObjectBase::ObjectBase(
        const UINT objectId,
        Trans*     pTrans)
        : m_objectId(objectId),
          m_pTrans(pTrans),
          m_enable(TRUE)
    {
    }

    ObjectBase::~ObjectBase()
    {
    }
}