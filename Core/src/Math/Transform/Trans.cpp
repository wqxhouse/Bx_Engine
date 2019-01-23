//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Trans.h"
#include "../Math.h"

void Trans::initialize()
{
    translationMatrix = Math::Translate(Math::Mat4::Identity(), pos);
    scaleMatrix       = Math::Scale(Math::Mat4::Identity(), scale);

    transMatrix = translationMatrix * scaleMatrix;

    viewMatrix = Math::viewMatrix(pos, front, up);
    
    // TODO: Rotation matrix
}

void Trans::update()
{
    if (m_transFlags.flags != 0)
    {
        Math::Mat4 translate = GetTranslateMatrix();
        Math::Mat4 scale     = GetScaleMatrix();

        // TODO: Rotation matrix

        if (m_transFlags.bits.transFlag == 1)
        {
            transMatrix = translate * scale;
        }

        if (m_transFlags.bits.viewFlag == 1)
        {
            viewMatrix = Math::viewMatrix(pos, pos + front, up);
        }

        // Reset all transform matrix flags
        m_transFlags.flags = 0;
    }
}

Math::Mat4 Trans::GetTransMatrix()
{
    update();

    return transMatrix;
}
