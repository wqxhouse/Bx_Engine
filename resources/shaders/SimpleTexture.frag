#version 440 core

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

uniform vec3 uniformColor;
uniform sampler2D sampler;

layout (std140) uniform light
{
	vec3 lightDir;
	vec3 lightColor;
};

uniform material
{
	vec4 ka;
	vec4 kd;
	vec4 ks;
	vec4 ns;
};

uniform vec3 eyePos;

out vec4 outColor;

void main()
{
	vec3 view = normalize(eyePos - posWorld);	
	
	float NoL = clamp(dot(normalWorld, -lightDir), 0.0f, 1.0f);
	vec3 reflection = normalize(2 * NoL * normalWorld + lightDir);
	
	float VoR = dot(view, reflection);
	if(VoR < 0.0f) VoR = 0.0f;
	float specularCoefficient = pow(VoR, ns.w);

	vec4 texColor = texture(sampler, fragTexCoord);
	vec3 diffuseColor = NoL * kd.xyz * lightColor;//fragColor * uniformColor;
	vec3 specColor = specularCoefficient * ks.xyz * lightColor;
	outColor = vec4((specColor + diffuseColor), 1.0f) * texColor;
	//outColor = vec4(normalWorld.xyz, 1.0f);
}