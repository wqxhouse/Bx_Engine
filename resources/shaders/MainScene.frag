#version 440 core

in vec3 posWorld;
in vec3 normalWorld;
in vec2 fragTexCoord;

in vec3 posLightProj;

uniform sampler2D sampler;
uniform sampler2D shadowMapSampler;

layout (std140) uniform light
{
	vec3 lightColor;
	vec3 lightDir;
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
	
	float VoR = clamp(dot(view, reflection), 0.0f, 1.0f);
	
	float specularCoefficient = pow(VoR, ns.w);

	float depth = texture(shadowMapSampler, posLightProj.xy).x;
	
	if (depth > gl_FragCoord.z - 0.00001f)
	{
		vec4 texColor = texture(sampler, fragTexCoord);
		vec3 diffuseColor = NoL * kd.xyz * lightColor;
		vec3 specColor = specularCoefficient * ks.xyz * lightColor;
		outColor = ka + vec4((specColor + diffuseColor), 1.0f) * texColor;
	}
	else
	{
		outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	//outColor = vec4(normalWorld.xyz, 1.0f);
}