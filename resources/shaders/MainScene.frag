#version 440 core

#include <Material.hglsl>

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

uniform sampler2D sampler;
uniform sampler2D shadowMapSampler;

layout (std140) uniform light
{
	vec3 lightColor;
	vec3 lightDir;
};

uniform material
{
	PhongMaterial m_phongMaterial;
};

uniform vec3 eyePos;

out vec4 outColor;

float castingShadow()
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
	vec2 depthUV = vec2(posLight.x * 0.5f + 0.5f, posLight.y * 0.5f + 0.5f);
	
	float depth = texture(shadowMapSampler, depthUV).x;
	
	if (depth < gl_FragCoord.z + 0.01f)
	{
		shadowAttenuation = 0.0f;
	}
	
	return shadowAttenuation;
}

void main()
{
	vec3 view = normalize(eyePos - posWorld);	
	
	float NoL = clamp(dot(normalWorld, -lightDir), 0.0f, 1.0f);
	vec3 reflection = normalize(2 * NoL * normalWorld + lightDir);
	
	float VoR = clamp(dot(view, reflection), 0.0f, 1.0f);
	
	float specularCoefficient = pow(VoR, m_phongMaterial.ks.w);
	
	float shadowAttenuation = castingShadow();

	vec4 texColor = texture(sampler, fragTexCoord);
	vec3 diffuseColor = NoL * m_phongMaterial.kd.xyz * lightColor;
	vec3 specColor = specularCoefficient * m_phongMaterial.ks.xyz * lightColor;
	outColor = (/*ka +*/ vec4((diffuseColor + specColor), 1.0f)) * texColor;
	
	//outColor *= shadowAttenuation;
	//outColor = vec4(normalWorld.xyz, 1.0f);
}