#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_DIRECTIONAL_LIGHT_NUM 16
#define MAX_POINT_LIGHT_NUM       16
#define MAX_SPOT_LIGHT_NUM        16

#define MAX_MESH_NUM              256

layout (location = 0) in vec3 posView;
layout (location = 1) in vec3 normalView;
layout (location = 2) in vec2 fragTexCoord;
layout (location = 3) in mat4 fragViewMat;

layout(location = 0) out vec4 outColor;

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

layout (binding = 1) uniform MaterialUbo
{
	vec4 materialIndex;
	vec4 padding[15];
};

layout (binding = 2) uniform LightData
{
    uint             directionalLightNum;
    DirectionalLight directionalLightList[MAX_DIRECTIONAL_LIGHT_NUM];

    uint             pointLightNum;
    PointLight       pointLightList[MAX_POINT_LIGHT_NUM];

    uint             spotLightNum;
    SpotLight        spotLightList[MAX_SPOT_LIGHT_NUM];
} m_lightData;

layout (binding = 3) uniform StaticUniform
{
    vec3 camPosWorld;
};

layout (binding = 4) uniform sampler2D AlbedoTexture[MAX_MESH_NUM];

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

    vec3 reflectVec  = normalize(2 * dot(N, -L) * N + L);

    vec3 camPosView  = (fragViewMat * vec4(camPosWorld, 1.0f)).xyz;
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
    vec3 normalizedNormalView = normalize(normalView);
    vec3 normalizedLightView  = normalize((fragViewMat * m_lightData.directionalLightList[0].direction).xyz);

    const uint albedoTextureIndex = floatBitsToUint(materialIndex.x);
	vec3 albedo = texture(AlbedoTexture[albedoTextureIndex], fragTexCoord).xyz;

    vec3 diffuseRadiance = calPhongDiffuseRadiance(
        normalizedNormalView,
        normalizedLightView,
        m_lightData.directionalLightList[0].lightBase.color.xyz,
        albedo);

    vec3 specularRadiance = calPhongSpecularRadiance(
        normalizedNormalView,
        normalizedLightView,
        m_lightData.directionalLightList[0].lightBase.color.xyz,
        vec3(0.6f),
        10.0f);

    vec3 radiance = (diffuseRadiance + specularRadiance);

    outColor = vec4(gammaCorrection(radiance), 1.0f);
}