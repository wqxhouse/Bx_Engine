/*
    MainScene.frag
*/

#version 440 core

#include <Light.hglsl>
#include <Material.hglsl>
#include <Utilities.hglsl>

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

uniform sampler2D sampler;
uniform sampler2D shadowMapSampler;
//uniform sampler2DMS shadowMapSampler;

uniform samplerCube lightProbeCubemap;

uniform shadowMapResolutionUniformBlock
{
    Resolution m_shadowMapResolution;
};

layout (std140) uniform lightArrayUniformBlock
{
    Light m_light[MAX_LIGHT_NUM];
};

layout (std140) uniform material
{
	PhongMaterial m_phongMaterial;
};

uniform vec3 eyePos;

uniform int lightNum;

out vec4 outColor;

float castingShadow()
{
	float shadowAttenuation = 1.0f;
	
	// Shadow casting
	vec3 posLight = posLightProj.xyz / posLightProj.w;
    posLight = posLight * 0.5f + 0.5f;
	
    // Multisampling, need integer coordinate
    //posLight.x = posLight.x * 2560.0f; // m_shadowMapResolution.width;
    //posLight.y = posLight.y * 2560.0f; // m_shadowMapResolution.height;
    
	float depth = texture(shadowMapSampler, posLight.xy).r;
    /*float depth = 0.0f;
    
    for (int i = 0; i < 4; ++i)
    {
        float pcfDepth = 0.0f;
        for (int j = -1; j < 1; ++j)
        {
            for(int k = -1; k < 1; ++k)
            {
                float tempPcfDepth = texelFetch(shadowMapSampler, ivec2(posLight.xy) + ivec2(j, k), i).r;                
                pcfDepth += ((tempPcfDepth < posLight.z - 0.000001f) ? 0.0f : tempPcfDepth);
            }
        }
        depth += (pcfDepth * 0.111111f); // pcfDepth / 9.0f
    }
    depth *= 0.25f;*/
	
	if (depth < posLight.z - 0.000001f)
	{
		shadowAttenuation = 0.2f;
	}
	
	return shadowAttenuation;
}

void main()
{
	for (int i = 0; i < lightNum; ++i)
	{
		DirectionalLight m_directionalLight;
		m_directionalLight.lightBase = m_light[i].lightBase;
		m_directionalLight.dir = m_light[i].data[0].xyz;
		
		vec3 view       = normalize(eyePos - posWorld);
		vec3 dir        = m_directionalLight.dir;
		vec3 lightColor = m_directionalLight.lightBase.color;
		
		float NoL = clamp(dot(normalWorld, -dir), 0.0f, 1.0f);
		vec3 reflection = normalize(2 * NoL * normalWorld + dir);
		
		float VoR = clamp(dot(view, reflection), 0.0f, 1.0f);
		
		float specularCoefficient = pow(VoR, m_phongMaterial.ks.w);
		
		float shadowAttenuation = castingShadow();
		float shadowSpecularAttenuation = ((shadowAttenuation < 0.9999999f) ? 0.0f : 1.0f);

		vec4 texColor = texture(sampler, fragTexCoord);
		
		// Light Probe
		vec3 environmentLight = texture(lightProbeCubemap, normalWorld).xyz;    
		lightColor += environmentLight;
		
		vec3 diffuseColor = clamp(NoL * m_phongMaterial.kd * lightColor, 0.0f, 1.0f);
		vec3 specColor    = clamp(specularCoefficient * m_phongMaterial.ks.xyz * lightColor, 0.0f, 1.0f);
		
		// Shadow casting(specular)
		specColor *= shadowSpecularAttenuation;
		
		vec3 outColorVec3 = clamp(((m_phongMaterial.ka + diffuseColor + specColor) * lightColor), 0.0f, 1.0f);// * texColor;
		
		// Shadow casting
		outColorVec3 *= shadowAttenuation;
		
		// Gamma correction
		outColorVec3 = gammaCorrection(outColorVec3);
		
		outColor = vec4(outColorVec3, 1.0f);
    }
}
