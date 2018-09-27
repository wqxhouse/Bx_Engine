#version 440 core

layout (local_size_x = 4) in;

layout(std140, binding = 0) buffer Frustums
{
    vec4 pos[];
};

void main()
{
    pos[gl_GlobalInvocationID.x] += vec4(0.1f, 0.2f, 0.3f, 1.0f);
}