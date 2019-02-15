#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 fragUV;

void main()
{
    fragUV = uv;

    gl_Position = vec4(position, 1.0f);
}