//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "Vector4.h"

// Defined as ax + by + cz +d = 0, vector N = (a, b ,c)
struct Plane
{
    Math::Vector3 N;
    float d;
};

struct Rectangle
{
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

struct Frustum
{
    Plane sidePlanes[4];

    Plane nearPlane;
    Plane farPlane;
};