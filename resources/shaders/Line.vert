/*
    Line.vert
*/

#version 440 core

layout (location = 0) in vec3 position;

uniform mat4 transMat;

void main()
{
    gl_Position = transMat * vec4(position, 1.0f);
}

// End of Line.vert