#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_DIRECTIONAL_LIGHT_NUM 16
#define MAX_POINT_LIGHT_NUM       16
#define MAX_SPOT_LIGHT_NUM        16

#define MAX_MESH_NUM              512
#define MAX_LIGHT_NUM             512

#define PI    3.1415926f
#define InvPI 0.318309886f

layout (location = 0) in vec3 posView;
layout (location = 1) in vec3 normalView;
layout (location = 2) in vec2 fragTexCoord;
layout (location = 3) in mat4 fragViewMat;

layout(location = 0) out vec4 outColor;

struct LightBase
{
    vec4 color;
    mat4 lightTransMat;
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
	vec4  materialIndex;
    float roughness;
    vec3  fresnel; float padding;
    float metallic;

	vec4  reserve[12];
} material;

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
    mat4 lightCamViewMat;
};

layout (binding = 4) uniform sampler2D AlbedoTexture[MAX_MESH_NUM];
layout (binding = 5) uniform sampler2D SpecularTexture[MAX_MESH_NUM];

// Shadow
layout (binding = 6) uniform sampler2D shadowMap[MAX_LIGHT_NUM];

/// BRDF functions
// GGX NDF
float calGGXNormalDistribution(const vec3  N,
                               const vec3  H,
                               const float alpha) ///< Material roughness
{
    float NDF = 0.0f;

    float alpha2 = alpha * alpha;

    float NoH  = clamp(dot(N, H), 0.0f, 1.0f);

    float NoH2 = NoH * NoH;

    float k  = NoH2 * (alpha2 - 1) + 1;
    float k2 = k * k;

    NDF = ((alpha2) / (PI * k2));

    return NDF;
}

// GGX Geometry function
float calGGXGeometry(const float NoX,   ///< NoL or NoV
                     const float alpha) ///< Material roughness
{
    float geometryCoefficient;
    
    float k = (alpha + 1) * (alpha + 1) * 0.125f;
    
    geometryCoefficient = ((NoX) / (NoX * (1 - k) + k));
    
    return geometryCoefficient;
}

// Schlick Fresnel with Spherical Gaussian approximation
// Reference: https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
vec3 schlickFresnel(const vec3 V,
                    const vec3 H,
                    const vec3 fresnel)
{
    vec3 fresnelCoefficient = vec3(1.0f);
    
    vec3 F0 = fresnel;
    
    float VoH      = clamp(dot(V, H), 0.0f, 1.0f);
    float exponent = (-5.55473f * VoH - 6.98316f) * VoH;
    
    fresnelCoefficient = F0 + (1 - F0) * pow(2.0f, exponent);
    
    return fresnelCoefficient;
}

// Calculate BRDF
vec3 calCooktorranceGGXBRDF(const vec3  V,
                            const vec3  N,
                            const vec3  L,
                            const float NoL,
                            const vec3  albedo)
{
    // Calculate diffuse BRDF with albedo model
    vec3 fd   = albedo * InvPI;

    // Calculate specular BRDF with Cook-Torrance model
    vec3 fs   = vec3(0.0f);

    float NoV = dot(N, V);

    vec3 ks = vec3(0.0f);
    vec3 kd = vec3(0.0f);

    if (NoL > 0.0f && NoV > 0.0f)
    {
        vec3 halfVector = normalize(V + L);
    
        float D = calGGXNormalDistribution(N, halfVector, material.roughness);
        float G = calGGXGeometry(NoL, material.roughness) * calGGXGeometry(NoV,  material.roughness);
        vec3  F = schlickFresnel(V, halfVector, material.fresnel);

        fs = (D * G * F) / (4.0f * NoL * NoV);

        ks = F;
        kd = (vec3(1.0f) - ks) * (1.0f - material.metallic);
    }

    // ks has already been calculated in fs with fresnel, no need to consider again
    vec3 brdf = clamp((kd * fd + fs), 0.0f, 1.0f);

    return brdf;
}

bool shadowCasting()
{
    bool isShadow = false;

    return isShadow;
}

vec3 gammaCorrection(
    const vec3 color)
{
    float gammaCorrectionExponent = 0.4545454545f; // 1.0f/2.2f
    vec3 outputColor = pow(color, vec3(gammaCorrectionExponent));

    return outputColor;
}

void main()
{
    vec3 normalizedNormalView = normalize(normalView);
    vec3 normalizedLightView  = normalize((fragViewMat * m_lightData.directionalLightList[0].direction).xyz);

    vec3 camPosView           = (fragViewMat * vec4(camPosWorld, 1.0f)).xyz;
    vec3 normalizedView       = normalize(camPosView - posView);

    const uint materialIndex = floatBitsToUint(material.materialIndex.x);
	vec3 albedo              = texture(AlbedoTexture[materialIndex], fragTexCoord).xyz;
    vec3 specular            = texture(SpecularTexture[materialIndex], fragTexCoord).xyz;

    float NoL = dot(normalizedNormalView, -normalizedLightView);

    vec3 radiance = m_lightData.directionalLightList[0].lightBase.color.xyz;
    vec3 brdf     = calCooktorranceGGXBRDF(normalizedView, normalizedNormalView, -normalizedLightView, NoL, albedo);

    vec3 irranciance = brdf * radiance * clamp(NoL, 0.0f, 1.0f);

    outColor = vec4(gammaCorrection(irranciance), 1.0f);
}