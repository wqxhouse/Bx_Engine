#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (input_attachment_index = 0, binding = 0) uniform subpassInput posWorldTexture;
layout (input_attachment_index = 1, binding = 1) uniform subpassInput normalWorldTexture;
layout (input_attachment_index = 2, binding = 2) uniform subpassInput texCoordTexture;

layout (location = 0) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D TestTexture;

void main()
{
    vec2 texCoord = subpassLoad(texCoordTexture).xy;
    
    outColor = vec4(texture(TestTexture, texCoord).xyz, 1.0f);
}