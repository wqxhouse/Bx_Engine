/*
    TextureBlur.frag
*/

#version 440 core

uniform sampler2D inputTexture;

out vec3 blurColor;

void main()
{
    vec2 texelSize = vec2(1.0f) / textureSize(inputTexture, 0);
    
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            vec2 sampleCoord = gl_FragCoord.xy;
            sampleCoord.x   += i;
            sampleCoord.y   += j;
            
            sampleCoord *= texelSize;
            
            blurColor += texture(inputTexture, sampleCoord).xyz;
        }
    }
    
    blurColor *= 0.1111111f; // blurColor * (1.0f / 9.0f)
}

// End TextureBlur.frag