#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 local_pos;

void main()
{
	gl_Position = vec4(local_pos, 1.0f);
}