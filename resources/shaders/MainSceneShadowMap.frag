#version 440 core

#include <Light.hglsl>

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

uniform sampler2D sampler;
uniform sampler2D shadowMapSampler;

layout (std140) uniform lightUniformBlock
{
    DirectLight light;
};

layout (std140) uniform material
{
	vec3 ka;
	vec3 kd;
	vec3 ks;
	vec4 ns;
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
	
	if (depth < gl_FragCoord.z + 0.000001f)
	{
		shadowAttenuation = 0.0f;
	}
	
	return shadowAttenuation;
}

void main()
{
	vec3 view = normalize(eyePos - posWorld);	
	
	float NoL = clamp(dot(normalWorld, -light.dir), 0.0f, 1.0f);
	vec3 reflection = normalize(2 * NoL * normalWorld + light.dir);
	
	float VoR = clamp(dot(view, reflection), 0.0f, 1.0f);
	
	float specularCoefficient = pow(VoR, ns.w);
	
	float shadowAttenuation = 1.0f; castingShadow();

	vec4 texColor = texture(sampler, fragTexCoord);
    
	vec3 diffuseColor = NoL * kd * light.lightBase.color;
	vec3 specColor = specularCoefficient * ks * light.lightBase.color;
	outColor = (vec4((/*ka + */diffuseColor + specColor), 1.0f)) * texColor;
    
    // Shadow casting
	outColor *= shadowAttenuation;
	//outColor = vec4(normalWorld.xyz, 1.0f);
}