#version 440 core

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

uniform vec3 uniformColor;
uniform sampler2D sampler;

/*layout (std140) uniform light
{
	vec4 lightDir;
	vec4 lightColor;
};*/

layout (std140) uniform light2
{
	vec4 lightDir;
	vec4 lightColor;
};

layout (std140) uniform material
{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float ns;
};

uniform vec3 eyePos;

out vec4 outColor;

void main()
{
	vec3 kd = vec3(0.4f, 0.4f, 0.4f);
	vec3 ks = vec3(0.6f, 0.6f, 0.6f);
	
	//float spec = 1000.0f;
	vec3 view = normalize(eyePos - posWorld);	
	
	float NoL = clamp(dot(normalWorld, -lightDir.xyz), 0.0f, 1.0f);
	vec3 reflection = normalize(2 * NoL * normalWorld + lightDir.xyz);
	
	float VoR = dot(view, reflection);
	if(VoR < 0.0f) VoR = 0.0f;
	float specularCoefficient = pow(VoR, 50.0f);

	vec4 texColor = texture(sampler, fragTexCoord);
	vec3 diffuseColor = NoL * kd * lightColor.xyz;//fragColor * uniformColor;
	vec3 specColor = specularCoefficient * ks * lightColor.xyz;
	outColor = vec4((specColor + diffuseColor) * texColor.xyz, 1.0f);
	//outColor = vec4(normalWorld.xyz, 1.0f);
}