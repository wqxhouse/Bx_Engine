#version 440 core

in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 uniformColor;
uniform sampler2D sampler;

out vec4 gl_Color;

void main()
{
	vec3 color3 = texture(sampler, fragTexCoord).xyz;//fragColor * uniformColor;
	gl_Color = vec4(fragNormal * color3.xyz, 1.0f);//texture(sampler, fragTexCoord);
	//gl_Color = vec4(fragNormal.xyz, 1.0f);
}