#version 440 core

#include <Light.hglsl>

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec4 posLightProj;

uniform sampler2D sampler;
uniform sampler2D shadowMapSampler;

layout (std140) uniform directionalLightUniformBlock
{
    DirectionalLight m_directionallight;
};

uniform pointLightUniformBlock
{
    PointLight m_pointLight;
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
	
	if (depth < gl_FragCoord.z + 0.0001f)
	{
		shadowAttenuation = 0.0f;
	}
	
	return shadowAttenuation;
}

void main()
{
    vec3  dis     = posWorld - m_pointLight.pos;
    float dis2    = dot(dis, dis);
    float radius2 = m_pointLight.radius * m_pointLight.radius;
    
    vec3 dir      = normalize(dis);
    
    if (dis2 <= radius2)
    {
        vec3 view = normalize(eyePos - posWorld);

        vec3 lightColor = m_pointLight.lightBase.color;
        
        //vec3 dir        = m_directionallight.dir;
        //vec3 lightColor = m_directionallight.lightBase.color;
        
        float NoL = clamp(dot(normalWorld, -dir), 0.0f, 1.0f);
        vec3 reflection = normalize(2 * NoL * normalWorld + dir);
        
        float VoR = clamp(dot(view, reflection), 0.0f, 1.0f);
        
        float specularCoefficient = pow(VoR, ns.w);
        
        float shadowAttenuation = castingShadow();

        vec4 texColor = texture(sampler, fragTexCoord);
        
        vec3 diffuseColor = NoL * kd * lightColor;
        vec3 specColor = specularCoefficient * ks * lightColor;
        outColor = (vec4((/*ka + */diffuseColor + specColor), 1.0f)) * texColor;
        
        // Shadow casting
        outColor *= shadowAttenuation;
        //outColor = vec4(normalWorld.xyz, 1.0f);
    }
    else
    {
        outColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}
