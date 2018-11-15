#version 450
#extension GL_ARB_separate_shader_objects : enable

// layout(location = 0) in vec3 local_pos;

vec2 vertexPos[3] = vec2[](
    vec2(0.0f, -0.5f), vec2(-0.5f, 0.5f), vec2(0.5f, 0.5f));

void main()
{
	gl_Position = vec4(vertexPos[gl_VertexIndex], 0.0f, 1.0f);
}