#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (binding = 0) uniform Trans
{
    mat4 padding[3];
	mat4 wvpMat;
};

void main()
{
    gl_Position = wvpMat * vec4(pos, 1.0f);
}