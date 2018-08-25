#pragma once

#include "../../Math/Vector3.h"
#include "Primitive.h"

class Quad : public Primitive
{
public:
    Quad();
    ~Quad();

private:
    Math::Vector3 m_quadVertices[4] =
    {
        { -1.0f, -1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
        {  1.0f, 1.0f, 1.0f }
    };

    UINT m_quadIndices[6] =
    {
        0, 1, 2,
        1, 3, 2
    };
};