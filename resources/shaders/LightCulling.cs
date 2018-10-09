/*
    LightCulling.cs
*/

#version 460 core

#extension GL_ARB_compute_variable_group_size : require
#extension GL_ARB_compute_variable_group_size : enable

#include <Utilities.hglsl>
#include <Light.hglsl>

#include <Frustum.hglsl>

layout (local_size_variable) in;

// Uniform blocks/variables
uniform uint frustumSize;

layout (std140) uniform globalSizeUniformBlock
{
    Resolution m_tileResolution;
};

uniform uint lightNum;
layout (std140) uniform lightArrayUniformBlock
{
    Light m_light[MAX_LIGHT_NUM];
};

uniform mat4 viewMat;

// Atomic counters
layout(binding = 0, offset = 0) uniform atomic_uint lightIndexListCounter;

// SSBO
layout(std430, binding = 0) buffer Frustums
{
    float   zNear;
    float   zFar;
    vec2    padding;

    Frustum m_frustum[];
};

struct LightTile
{
    uint offset;
    uint size;
};

layout(std430, binding = 1) buffer LightGridBuffer
{
    LightTile lightGrid[];
};

layout(std430, binding = 2) buffer LightIndexListBuffer
{
    uint lightIndexList[];
};

bool sphereOutsidePlane(
    Plane p, // The plane
    vec3  c, // The center of sphere
    float r) // The radius of sphere
{
    return (dot(p.N, c) - p.d < -r);
}

// Sphere-Frustum culling function
bool pointLightInsideFrustum(
    Frustum threadFrustum, // The frustum in current thread
    vec3    center,        // The center of point light
    float   radius)        // The radius of point light
{
    bool result = true;

    // The point light is outside of the view near/far clip space
    if ((center.z - radius < zNear) ||
        (center.z + radius > zFar))
    {
        result = false;
    }
    
    for (uint i = 0; ((i < 4) && (result == true)); ++i)
    {
        if (sphereOutsidePlane(threadFrustum.m_plane[i], center, radius) == true)
        {
            result = false;
        }
    }

    return result;
}

bool spotLightInsideFrustum(
    Frustum threadFrustum, // The frustum in current thread
    vec3    pos,           // The position of spot light
    vec3    direction,     // The direction of spot light
    float   outerCosTheta) // The outer angle cosine value
{
    bool result = true;

    return result;
}

bool lightVisibility(uint frustumIndex, uint lightIndex)
{
    bool lightVisible = true;

    Frustum frustum = m_frustum[frustumIndex];
    Light   light   = m_light[lightIndex];

    switch(light.lightBase.type)
    {
        case 0: // Directional Light
        {
            break;
        }
        case 1: // Point Light
        {
            vec4 pointLightCenterVSHomogeneous = viewMat * vec4(light.data[0].xyz, 1.0f);
            vec3 pointLightCenterVS = pointLightCenterVSHomogeneous.xyz / pointLightCenterVSHomogeneous.w;

            lightVisible = pointLightInsideFrustum(frustum, pointLightCenterVS, light.data[0].w);
            break;
        }
        case 2: // Spot Light
        {
            vec4 pointLightCenterVSHomogeneous = viewMat * vec4(light.data[0].xyz, 1.0f);
            vec3 pointLightCenterVS = pointLightCenterVSHomogeneous.xyz / pointLightCenterVSHomogeneous.w;

            lightVisible = spotLightInsideFrustum(frustum, pointLightCenterVS, light.data[1].xyz, light.data[1].w);
            break;
        }
        default:
            break;
    }

    return lightVisible;
}

void main()
{
    uint threadId = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * m_tileResolution.width;

    if (threadId < frustumSize)
    {
        uint tileLightSize = 0;
        uint localLightIndexList[MAX_LIGHT_NUM_TILE];

        for (uint i = 0; i < lightNum; ++i)
        {
            bool isLightVisible = lightVisibility(threadId, i);

            if (isLightVisible == true)
            {
                localLightIndexList[tileLightSize] = i;
                tileLightSize++;
            }
        }

        lightGrid[threadId].offset = atomicCounterAdd(lightIndexListCounter, tileLightSize);
        lightGrid[threadId].size   = tileLightSize;

        // Copy the local light index list to global list
        for (uint i = 0; i < tileLightSize; ++i)
        {
            uint globalLightIndexListIndex            = lightGrid[threadId].offset + i;
            lightIndexList[globalLightIndexListIndex] = localLightIndexList[i];
        }
    }
}

// LightCulling.cs