#version 440 core

#extension GL_ARB_compute_variable_group_size : require
#extension GL_ARB_compute_variable_group_size : enable

#include <Utilities.hglsl>

layout (local_size_variable) in;
// layout(local_size_x = 4) in;

uniform int frustumSize;

uniform int groupSize;

uniform mat4 projMatInv;

uniform globalSizeUniformBlock
{
    Resolution m_tileResolution;
};

uniform forwardPlusResolutionUniformBlock
{
    Resolution m_forwardPlusResolution;
};

struct Plane
{
    vec3 N;
    float d;
};

struct Frustum
{
    Plane m_plane[4];
};

layout(std140, binding = 0) buffer Frustums
{
    Frustum frustum[];
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

    vec3 v1 = (p1 - p0);
    vec3 v2 = (p2 - p0);
    
    plane.N = cross(v1, v2);
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
        screenSpaceFrustumFarPlane[2] = vec4(gl_GlobalInvocationID.x * groupSize, gl_GlobalInvocationID.y * groupSize, -1.0f, 1.0f);
        screenSpaceFrustumFarPlane[3] = vec4((gl_GlobalInvocationID.xy + ivec2(1)) * groupSize, -1.0f, 1.0f);
        
        vec3 viewSpaceFrustumFarPlane[4];
        for (int i = 0; i < 4; ++i)
        {
            viewSpaceFrustumFarPlane[i] = screenToView(screenSpaceFrustumFarPlane[i]);
        }
            
        vec3 eyePos = vec3(0.0f);
        
        for (int i = 0; i < 3; ++i)
        {
            frustum[threadId].m_plane[i] = computeFrustum(eyePos, viewSpaceFrustumFarPlane[i], viewSpaceFrustumFarPlane[i + 1]);
        }
        frustum[threadId].m_plane[3] = computeFrustum(eyePos, viewSpaceFrustumFarPlane[3], viewSpaceFrustumFarPlane[0]);
    }
}