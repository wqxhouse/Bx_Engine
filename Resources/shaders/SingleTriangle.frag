#version 440 core

in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 uniformColor;
uniform sampler2D sampler;

out vec4 gl_Color;

void main()
{
	//vec3 color3 = texture(sampler, fragTexCoord);//fragColor * uniformColor;
	gl_Color = vec4(fragNormal.xyz, 1.0f);//texture(sampler, fragTexCoord);//vec4(color3.xyz, 1.0f);
}