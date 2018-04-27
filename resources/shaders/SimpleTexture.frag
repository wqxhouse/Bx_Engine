#version 440 core

in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 uniformColor;
uniform sampler2D sampler;

uniform vec3 eye;

uniform light
{
	vec3 lightDir;
	vec3 lightColor;
};

out vec4 outColor;

void main()
{
	float spec = 50.0f;
	
	float NoR = clamp(dot(eye, lightDir), 0.0f, 1.0f);
	vec3 reflection = 2 * NoR * (-lightDir) + lightDir;
	float VoR = clamp(dot(-eye, reflection), 0.0f, 1.0f);
	float phongCoefficient = pow(VoR, spec);
	
	float NoL = clamp(dot(fragNormal, -lightDir), 0.0f, 1.0f);

	vec3 color3 = texture(sampler, fragTexCoord).xyz * (NoL + phongCoefficient) * lightColor;//fragColor * uniformColor;
	outColor = vec4(color3.xyz, 1.0f);//texture(sampler, fragTexCoord);
	//outColor = vec4(fragNormal.xyz, 1.0f);
}