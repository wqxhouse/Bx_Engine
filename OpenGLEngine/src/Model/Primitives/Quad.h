//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../../Math/Vector3.h"
#include "Primitive.h"

class Quad : public Primitive
{
public:
    Quad(
        const Math::Vector2& pos  = Math::Vector2(),
        const float          size = 1.0f);
    ~Quad();

private:
    Math::Vector3 m_quadVertices[4] =
    {
        { -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f }
    };

    UINT m_quadIndices[6] =
    {
        0, 1, 2,
        1, 3, 2
    };
};