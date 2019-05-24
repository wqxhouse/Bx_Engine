#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_DIRECTIONAL_LIGHT_NUM 16
#define MAX_POINT_LIGHT_NUM       16
#define MAX_SPOT_LIGHT_NUM        16

layout (input_attachment_index = 0, binding = 0) uniform subpassInput posViewTexture;
layout (input_attachment_index = 1, binding = 1) uniform subpassInput normalViewTexture;
layout (input_attachment_index = 2, binding = 2) uniform subpassInput albedoTexture;
layout (input_attachment_index = 3, binding = 3) uniform subpassInput specularTexture;

layout (location = 0) out vec4 outColor;

struct LightBase
{
    vec4 color;
    mat4 lightTransVP;
};

struct DirectionalLight
{
    LightBase lightBase;

    vec4 direction;
};

struct PointLight
{
    LightBase lightBase;

    vec3  pos;
    float range;
};

struct SpotLight
{
    LightBase lightBase;

    vec3  pos;
    float innerCosTheta;

    vec3  direction;
    float outerCosTheta;
};

layout (binding = 4) uniform LightData
{
    uint             directionalLightNum;
    DirectionalLight directionalLightList[MAX_DIRECTIONAL_LIGHT_NUM];

    uint             pointLightNum;
    PointLight       pointLightList[MAX_POINT_LIGHT_NUM];

    uint             spotLightNum;
    SpotLight        spotLightList[MAX_SPOT_LIGHT_NUM];
} m_lightData;

layout (binding = 5) uniform CamPosUniform
{
    vec3 camPosWorld;
};

layout (binding = 6) uniform ViewMatUniform
{
    mat4 viewMat;
};

// layout (binding = 7) uniform EmptyUniform {};

// Calculate the diffuse radiance for phong shading
// N(normal), L(light direction) must be normalized
vec3 calPhongDiffuseRadiance(
    const vec3 N,
    const vec3 L,
    const vec3 lightColor,
    const vec3 kd)
{
    vec3 diffuseRadiance = kd * max(dot(N, -L), 0.0f) * lightColor;
    return diffuseRadiance;
}

// Calculate the specular radiance for phong shading
// N(normal), L(light direction) and V(view direction) must be normalized
vec3 calPhongSpecularRadiance(
    const vec3  N,
    const vec3  L,
    const vec3  lightColor,
    const vec3  ks,
    const float specularIntense)
{
    vec3 specularRadiance;

	vec3 posView    = subpassLoad(posViewTexture).xyz;

    vec3 reflectVec  = normalize(2 * dot(N, -L) * N + L);

	vec3 camPosView  = (viewMat * vec4(camPosWorld, 1.0f)).xyz;
    vec3 V           = normalize(camPosView - posView);

    float VoR        = max(dot(V, reflectVec), 0.0f);

    specularRadiance = ks * lightColor * pow(VoR, specularIntense);

    return specularRadiance;
}

vec3 gammaCorrection(
    const vec3 color)
{
    float gammaCorrectionExponent = 1.0f / 2.2f;
    vec3 outputColor = pow(color, vec3(gammaCorrectionExponent));

    return outputColor;
}

void main()
{
	vec3 normalizedNormalView = normalize(subpassLoad(normalViewTexture).xyz);
    vec3 normalizedLightView  = normalize((viewMat * m_lightData.directionalLightList[0].direction).xyz);

    vec3 albedo               = subpassLoad(albedoTexture).xyz;
    vec4 specular             = subpassLoad(specularTexture);

    vec3 diffuseRadiance = calPhongDiffuseRadiance(
        normalizedNormalView,
        normalizedLightView,
        m_lightData.directionalLightList[0].lightBase.color.xyz,
        albedo);

	vec3 specularRadiance = calPhongSpecularRadiance(
        normalizedNormalView,
        normalizedLightView,
        m_lightData.directionalLightList[0].lightBase.color.xyz,
        specular.xyz,
        specular.w);

    vec3 radiance = diffuseRadiance + specularRadiance;

    outColor = vec4(gammaCorrection(radiance), 1.0f);
}