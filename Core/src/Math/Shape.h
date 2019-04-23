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

struct Ray
{
    Math::Vector3 origin;
    Math::Vector3 dir;
};

// Defined as ax + by + cz +d = 0, vector N = (a, b ,c)
struct Plane
{
    Math::Vector3 N;
    float d;
};

struct Triangle
{
    Math::Vector3 v0;
    Math::Vector3 v1;
    Math::Vector3 v2;
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

struct Sphere
{
    Math::Vector3 center;
    float         radius;
};

struct Frustum
{
    Plane sidePlanes[4];

    Plane nearPlane;
    Plane farPlane;
};