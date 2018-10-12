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

uniform uint lightNum;
layout (std140) uniform lightArrayUniformBlock
{
    Light m_light[MAX_LIGHT_NUM];
};

uniform mat4 viewMat;
uniform mat4 projMatInv;

// Atomic counters
layout(binding = 0, offset = 0) uniform atomic_uint lightIndexListCounter;

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

vec3 screenToView(vec4 screenPos)
{
    // Screen space to clip space
    vec4 clipPos =
        vec4(screenPos.x / m_forwardPlusResolution.width, screenPos.y / m_forwardPlusResolution.height, screenPos.z, 1.0f);

    clipPos.x = -2.0f * clipPos.x + 1.0f;
    clipPos.y =  2.0f * clipPos.y - 1.0f;

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

    // The point light is outside of the view near/far clip space
    if ((center.z - radius > zNearVS/*threadFrustum.nearPlane.d*/) ||
        (center.z + radius < zFarVS/*threadFrustum.farPlane.d*/))
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
    uint threadId = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * m_tileResolution.width;

    if (threadId < frustumSize)
    {
        // Calculate near/far plane for the frustum
        float minDepth = -zFarVS;
        float maxDepth = -zNearVS;

        ivec2 iTexCoordBase = ivec2(gl_GlobalInvocationID.x * m_tileSize.width,
                                    gl_GlobalInvocationID.y * m_tileSize.height);

        for (uint i = 0; i < m_tileSize.width; ++i)
        {
            for (int j = 0; j < m_tileSize.height; ++j)
            {
                ivec2 iTexCoord = iTexCoordBase + ivec2(float(i), float(j));
                vec2  texCoord  = vec2(float(iTexCoord.x) / float(m_renderingResolution.width),
                                       float(iTexCoord.y) / float(m_renderingResolution.height));

                float depthScreen = texture(depthTexture, texCoord).r;
                float depth       = -screenToView(vec4(0.0f, 0.0f, depthScreen, 1.0f)).z;

                minDepth = min(minDepth, depth);
                maxDepth = max(maxDepth, depth);
            }
        }

        m_frustum[threadId].nearPlane.N = vec3(0.0f, 0.0f, -1.0f);
        m_frustum[threadId].nearPlane.d = -minDepth;

        m_frustum[threadId].farPlane.N  = vec3(0.0f, 0.0f, 1.0f);
        m_frustum[threadId].farPlane.d  = -maxDepth;

        barrier();
        
        // Light culling
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

        barrier();
        
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