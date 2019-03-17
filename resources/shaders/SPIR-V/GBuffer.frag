#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 posView;
layout (location = 1) in vec3 normalView;
layout (location = 2) in vec2 fragTexCoord;

layout (location = 0) out vec3 posViewTexture;
layout (location = 1) out vec3 normalViewTexture;
layout (location = 2) out vec4 albedoTexture;

layout (binding = 1) uniform sampler2D TestTexture;

// TODO
// layout (locatoin = 3) out vec3 albedoTexture;

void main()
{
    posViewTexture    = posView;
    normalViewTexture = normalView;
    albedoTexture     = texture(TestTexture, fragTexCoord);
}