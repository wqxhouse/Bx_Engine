/*
    GBuffer.vert
*/
#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 wv;
uniform mat4 wvp;

uniform mat4 lightTransWVP;

out vec3 posView;
out vec3 normalView;

out vec2 fragTexCoord;

out vec4 posLightProj;

void main()
{
    vec4 posViewVec4 = wv * vec4(position, 1.0f);
    posView          = posViewVec4.xyz / posViewVec4.w;

    normalView   = (wv * vec4(normal, 0.0f)).xyz;

    fragTexCoord = texCoord;
    
    posLightProj = lightTransWVP * vec4(position, 1.0f);
    
    gl_Position  = wvp * vec4(position, 1.0f);
}
