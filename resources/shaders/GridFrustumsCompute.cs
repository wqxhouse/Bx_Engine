/*
    GridFrustumsCompute.cs
*/

#version 460 core

#extension GL_ARB_compute_variable_group_size : require
#extension GL_ARB_compute_variable_group_size : enable

#include <Utilities.hglsl>
#include <Frustum.hglsl>

layout (local_size_variable) in;

uniform int frustumSize;

uniform int groupSize;

uniform mat4 projMatInv;

uniform GlobalSizeUniformBlock
{
    Resolution m_tileResolution;
};

uniform ForwardPlusResolutionUniformBlock
{
    Resolution m_forwardPlusResolution;
};

layout(std430, binding = 0) buffer Frustums
{
// Frustum 0, only for holding zNear/zFar
    float   zNearVS;
    float   zFarVS;
    vec2    padding;
    Plane   paddingPlanes[5];

// Actual screen frustums start from frustum[0]
    Frustum m_frustum[];
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

Plane computeFrustum(vec3 p0, vec3 p1, vec3 p2)
{
    Plane plane;

    vec3 v1 = normalize(p1 - p0);
    vec3 v2 = normalize(p2 - p0);
    
    plane.N = normalize(cross(v1, v2));
    plane.d = dot(plane.N, p0);
    
    return plane;
}

void main()
{    
    uint threadId = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * m_tileResolution.width;

    if (threadId < frustumSize)
    {
        vec4 screenSpaceFrustumFarPlane[4];
        
        screenSpaceFrustumFarPlane[0] = vec4(gl_GlobalInvocationID.xy * groupSize, -1.0f, 1.0f);
        screenSpaceFrustumFarPlane[1] = vec4((gl_GlobalInvocationID.x + 1) * groupSize, gl_GlobalInvocationID.y * groupSize, -1.0f, 1.0f);
        screenSpaceFrustumFarPlane[2] = vec4((gl_GlobalInvocationID.xy + ivec2(1)) * groupSize, -1.0f, 1.0f);
        screenSpaceFrustumFarPlane[3] = vec4(gl_GlobalInvocationID.x * groupSize, (gl_GlobalInvocationID.y + 1) * groupSize, -1.0f, 1.0f);

        vec3 viewSpaceFrustumFarPlane[4];
        for (int i = 0; i < 4; ++i)
        {
            viewSpaceFrustumFarPlane[i] = screenToView(screenSpaceFrustumFarPlane[i]);
        }
            
        vec3 eyePos = vec3(0.0f);
        
        for (int i = 0; i < 3; ++i)
        {
            m_frustum[threadId].m_plane[i] = computeFrustum(eyePos, viewSpaceFrustumFarPlane[i], viewSpaceFrustumFarPlane[i + 1]);
        }
        m_frustum[threadId].m_plane[3] = computeFrustum(eyePos, viewSpaceFrustumFarPlane[3], viewSpaceFrustumFarPlane[0]);
    }
}

// GridFrustumsCompute.cs