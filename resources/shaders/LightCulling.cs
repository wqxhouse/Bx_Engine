/*
    LightCulling.cs
*/

#version 440 core

#extension GL_ARB_compute_variable_group_size : require
#extension GL_ARB_compute_variable_group_size : enable

#include <Utilities.hglsl>
#include <Frustum.hglsl>

layout (local_size_variable) in;

uniform globalSizeUniformBlock
{
    Resolution m_tileResolution;
};

struct LightTile
{
    uint offset;
    uint size;
};

layout(std430, binding = 0) buffer Frustums
{
    Frustum frustum[];
};

layout(std430, binding = 1) buffer LightGridBuffer
{
    LightTile lightGrid[];
};

layout(std430, binding = 2) buffer LightIndexListBuffer
{
    uint lightIndexList[];
};

void main()
{
    uint threadId = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * m_tileResolution.width;
}

// LightCulling.cs