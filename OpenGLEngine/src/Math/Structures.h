#pragma once

#include "Vector4.h"

struct BxsRectangle
{
    BxsRectangle(const float left,
              const float right,
              const float bottom,
              const float top)
    {
        this->left   = left;
        this->right  = right;
        this->bottom = bottom;
        this->top    = top;
    }

    float left;
    float right;
    float bottom;
    float top;
};

struct AABB
{
    Math::Vector3 maxPoint;
    Math::Vector3 minPoint;
};