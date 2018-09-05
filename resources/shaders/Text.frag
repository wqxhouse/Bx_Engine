/*
    Text.frag
*/

#version 440 core

uniform sampler2D fontTex;

in vec2 fragUV;

out vec4 outColor;

void main()
{
    vec4 texColor = texture(fontTex, fragUV);
    
    float threshold = 0.5f;
    if (texColor.x < threshold &&
        texColor.y < threshold &&
        texColor.z < threshold)
    {
        texColor.w = 0.0f;
    }

    outColor = texColor;
}

// End of Text.frag