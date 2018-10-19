/*
    LightCulling.cs
*/

#version 450 core

// #extension GL_ARB_compute_variable_group_size : require
// #extension GL_ARB_compute_variable_group_size : enable

#include <Utilities.hglsl>
#include <Light.hglsl>

#include <Frustum.hglsl>

#line 14

// layout (local_size_variable) in;
layout (local_size_x = 16, local_size_y = 16) in;

// Scene Depth Texture
uniform sampler2D depthTexture;

// Uniform blocks/variables
uniform uint frustumSize;

layout (std140) uniform RenderingResolutionBlock
{
    Resolution m_renderingResolution;
};

layout (std140) uniform GlobalSizeUniformBlock
{
    Resolution m_tileResolution;
};

layout (std140) uniform TileSizeUniformBlock
{
    Resolution m_tileSize; // 16 * 16 by default
};

uniform ForwardPlusResolutionUniformBlock
{
    Resolution m_forwardPlusResolution;
};

uniform mat4 viewMat;
uniform mat4 projMatInv;

// Atomic counters
layout(binding = 0, offset = 0) uniform atomic_uint lightIndexListCounter;

// Group shared variables
shared uint localLightIndexList[MAX_LIGHT_NUM_TILE];
shared uint tileLightSize;
shared uint uMinDepth;
shared uint uMaxDepth;

// Shader global variables
float minDepth;
float maxDepth;
float tileDepthDis;
uint  tileIndex;

// 2.5D culling
shared uint tileDepthMask;

// SSBO
layout(std430, binding = 0) buffer Frustums
{
    float   zNearVS;
    float   zFarVS;
    vec2    padding;
    Plane   padding2[5];

    Frustum m_frustum[];
};

layout(std430, binding = 1) buffer LightGridBuffer
{
    LightTile lightGrid[];
};

layout(std430, binding = 2) buffer LightIndexListBuffer
{
    uint lightIndexList[];
};

// Test
uniform uint enableAdvancedCulling;

layout(std430, binding = 4) buffer LightIndexCounter
{
    uint m_counter;
};

vec3 screenToView(vec4 screenPos)
{
    // Screen space to clip space
    vec4 clipPos = vec4(screenPos.x / m_forwardPlusResolution.width,
                        screenPos.y / m_forwardPlusResolution.height,
                        screenPos.z,
                        1.0f);

    clipPos.x = -2.0f * clipPos.x + 1.0f;
    clipPos.y =  2.0f * clipPos.y - 1.0f;
    clipPos.z =  2.0f * clipPos.z - 1.0f;

    // Clip space to view space
    vec4 viewPos = projMatInv * clipPos;
    viewPos.xyz /= viewPos.w;

    return viewPos.xyz;
}

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

    float centerDepth = -center.z;

    /// 2.5D culling
    float minLightDepth     = centerDepth - radius;
    float maxLightDepth     = centerDepth + radius;
    uint minLightDepthIndex = min(31, max(0, uint(floor((minLightDepth - minDepth) / tileDepthDis))));
    uint maxLightDepthIndex = min(31, max(0, uint(floor((maxLightDepth - minDepth) / tileDepthDis))));

    // Set the bits in light mask from minLightDepthIndex to maxLightDepthIndex to 1
    // E.g. min = 2, max = 5;
    // So the minMask is 1 << 2 = 100, maxMask is 1 << 5 = 100000;
    // 100000 - 1 = 11111, 100 - 1 = 11;
    // 11111 ^ 11 = 11100;
    // 100000 | 11100 = 111100
    uint lightMask = ((1 << maxLightDepthIndex) |
                      (((1 << maxLightDepthIndex) - 1) ^
                       ((1 << minLightDepthIndex) - 1)));

    if ((enableAdvancedCulling == 1) && ((lightMask & tileDepthMask) == 0))
    {
        result = false;
    }
    /// End 2.5D culling

    // The point light is outside of the view near/far clip space
    if ((result == true) &&
        ((centerDepth + radius < threadFrustum.nearPlane.d) ||
         (centerDepth - radius > threadFrustum.farPlane.d)))
    {
        // result = false;
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
    Light   light   = m_lightBuffer.m_light[lightIndex];

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
        case 2: // TODO: Spot Light
        {
            vec4 spotLightCenterVSHomogeneous = viewMat * vec4(light.data[0].xyz, 1.0f);
            vec3 spotLightCenterVS = spotLightCenterVSHomogeneous.xyz / spotLightCenterVSHomogeneous.w;

            lightVisible = spotLightInsideFrustum(frustum, spotLightCenterVS, light.data[1].xyz, light.data[1].w);
            break;
        }
        default:
            break;
    }

    return lightVisible;
}

void main()
{
    uint frustumIndex     = gl_WorkGroupID.x + gl_WorkGroupID.y * m_tileResolution.width;
    uint localThreadIndex = gl_LocalInvocationID.x + gl_LocalInvocationID.y * m_tileSize.width;

    if (frustumIndex < frustumSize)
    {
        /// Calculate near/far (max/min) plane for the frustum
        // Initialization
        if (localThreadIndex == 0)
        {
            tileLightSize = 0;
            uMinDepth     = 0xFFFFFFFF;
            uMaxDepth     = 0;
            tileDepthMask = 0;
        }

        barrier();

        // Calculate the max/min for near/far plane
        ivec2 iTexCoord = ivec2(gl_GlobalInvocationID.xy);
        vec2  texCoord  = vec2(float(m_renderingResolution.width - iTexCoord.x) / float(m_renderingResolution.width),
                               float(iTexCoord.y) / float(m_renderingResolution.height));

        float depthScreen   =  texture(depthTexture, texCoord).r;
        float depth         = -screenToView(vec4(0.0f, 0.0f, depthScreen, 1.0f)).z;
        uint  uDepth        =  floatBitsToUint(depth);

        // TODO: Use Parallel Reduction here, avoid atomic overhead
        atomicMin(uMinDepth, uDepth);
        atomicMax(uMaxDepth, uDepth);
        // End max/min calculation

        barrier();

        minDepth = uintBitsToFloat(uMinDepth);
        maxDepth = uintBitsToFloat(uMaxDepth);

        /// 2.5D culling tile depth mask construction
        tileDepthDis = ((maxDepth - minDepth) * 0.03125f);
        tileIndex    = min(31, max(0, uint(floor((depth - minDepth) / tileDepthDis))));

        atomicOr(tileDepthMask, (1 << tileIndex));
        /// End 2.5D culling tile depth mask construction

        m_frustum[frustumIndex].nearPlane.N = vec3(0.0f, 0.0f, -1.0f);
        m_frustum[frustumIndex].nearPlane.d = minDepth;

        m_frustum[frustumIndex].farPlane.N  = vec3(0.0f, 0.0f, 1.0f);
        m_frustum[frustumIndex].farPlane.d  = maxDepth;
        /// End far/near plane calculation

        barrier();

        // Light culling
        uint tileBlockSize = m_tileSize.width * m_tileSize.height;

        for (uint i = localThreadIndex; i < m_lightBuffer.lightNum; i+= tileBlockSize)
        {
            bool isLightVisible = lightVisibility(frustumIndex, i);

            if (isLightVisible == true)
            {
                uint tileLightIndex                 = atomicAdd(tileLightSize, 1);
                localLightIndexList[tileLightIndex] = i;
            }
        }

        barrier();
        
        lightGrid[threadId].offset = atomicAdd(m_counter, tileLightSize); //atomicCounterAdd(lightIndexListCounter, tileLightSize);
        lightGrid[threadId].size   = tileLightSize;

        if (localThreadIndex == 0)
        {
            lightGrid[frustumIndex].offset = atomicCounterAdd(lightIndexListCounter, tileLightSize);
            lightGrid[frustumIndex].size   = tileLightSize;

            // Copy the local light index list to global list
            for (uint i = 0; i < tileLightSize; ++i)
            {
                uint globalLightIndexListIndex            = lightGrid[frustumIndex].offset + i;
                lightIndexList[globalLightIndexListIndex] = localLightIndexList[i];
            }
        }
    }
}

// LightCulling.cs