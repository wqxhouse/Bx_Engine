/*
    GBuffer.frag
*/
#version 440 core

#include <Material.hglsl>
#include <Utilities.hglsl>

layout (std140) uniform gMaterial
{
	PhongMaterial m_phongMaterial;
};

layout (std140) uniform gMaterialPBR
{
	CookTorranceMaterial m_pbrMaterial;
};

uniform int materialType;

uniform vec3 cameraPos;

in vec4 posWorldVec4;
in vec3 posWorld;
in vec3 normalWorld;

in vec3 posView;
in vec3 normalView;

in vec2 fragTexCoord;

in vec4 posLightProj;

// layout(location = 0) out vec4 posViewTexture;
layout(location = 0) out vec4 posWorldTexture;
layout(location = 1) out vec3 normalViewTexture;
layout(location = 2) out vec2 texCoordTexture;
layout(location = 3) out vec4 albedoTexture;
layout(location = 4) out vec4 specularTexture;
layout(location = 5) out vec3 environmentLightTexture;
layout(location = 6) out vec4 posViewTexture;

uniform int modelIndex;

void main()
{
    //float shadowAttenuation = 1.0f; //castingShadow();

    // posViewTexture     = vec4(posView, 1.0f);
    posWorldTexture    = posWorldVec4; //vec4(posWorld, 1.0f);
    normalViewTexture  = normalView;
    texCoordTexture    = fragTexCoord;
    posViewTexture     = vec4(posView, 1.0f);

    switch (materialType)
    {
        case PHONG_MATERIAL:
        {
            albedoTexture            = vec4(m_phongMaterial.kd, 1.0f);
            specularTexture          = m_phongMaterial.ks;
            environmentLightTexture  = m_phongMaterial.ka;
            break;
        }
        case ALBEDO_MATERIAL:
        // TODO
            break;
        case COOKTORRANCE_MATERIAL:
        {
            albedoTexture           = vec4(m_pbrMaterial.albedo, 1.0f);
            specularTexture         = vec4(m_pbrMaterial.roughness, m_pbrMaterial.metallic, m_pbrMaterial.fresnel, -1.0f);

            // Store the world space reflection vector for sampling light probe
            vec3 view       = normalize(cameraPos - posWorld);
            vec3 normal     = normalize(normalWorld);

            vec3 reflection = normalize(2 * dot(normal, view) * normal - view);

            environmentLightTexture = reflection;
        }
        default:
            break;
    }
}
// End GBuffer.frag