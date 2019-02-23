#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (input_attachment_index = 0, binding = 0) uniform subpassInput posViewTexture;
layout (input_attachment_index = 1, binding = 1) uniform subpassInput normalViewTexture;
layout (input_attachment_index = 2, binding = 2) uniform subpassInput texCoordTexture;

layout (binding = 3) uniform sampler2D TestTexture;

layout (location = 0) out vec4 outColor;

void main()
{
	vec3 posView    = subpassLoad(posViewTexture).xyz;
	vec3 normalView = subpassLoad(normalViewTexture).xyz;
    vec2 texCoord   = subpassLoad(texCoordTexture).xy;
    
    outColor = vec4(texture(TestTexture, texCoord).xyz, 1.0f);
}