#pragma once

#include "../../Core/TypeDef.h"
#include "../../Math/Matrix4x4.h"

class Texture2D;

class Material
{
public:
	Material(
        const MaterialType materialType,
        const std::string& materialName,
        const float refraction,
        const float alpha,
        const bool transparency);

	void setShadingMethod(const ShadingMethod shadingMethod);
	virtual ~Material() {}

    inline MaterialType GetMaterialType() const { return m_materialType; }

private:
	std::string m_materialName;
    //UINT        m_materialId;

	MaterialType m_materialType;
	ShadingMethod m_shadingMethod;

	bool transparency;
	float refraction;
	float alpha;
};

class SpecularMaterial : public Material
{
public:
	SpecularMaterial(const std::string& materialName);
	SpecularMaterial(const std::string& materialName,
		             const Math::Vector3& ka, const Math::Vector3& kd, const Math::Vector3& ks, float ns);
	SpecularMaterial(const std::string& materialName,
		             const Math::Vector3& ka, const Math::Vector3& kd, const Math::Vector3& ks,
					 float ns, float refraction, float alpha, bool transparency);

	~SpecularMaterial() {}

    void operator=(const SpecularMaterial& material)
    {
        m_specularMaterialData = material.m_specularMaterialData;
    }

    union
    {
        struct
        {
            Math::Vector3 ka; float ka_padding;
            Math::Vector3 kd; float kd_padding;
            Math::Vector3 ks; float ns; // Vector4
            Math::Vector4 padding;
        };
        Math::Mat4 m_specularMaterialData;
    };
};

class CookTorranceMaterial : public Material
{
public:
    // TODO: Load material from file
    CookTorranceMaterial()
        : Material(COOKTORRANCE, "", 1.0f, 1.0f, false),
          albedoVector4(Math::Vector4(1.0f)),
          roughness(0.5f),
          metallic(0.5f),
          fresnel(1.0f)
    {

    }

    ~CookTorranceMaterial() {}

    inline void* GetCookTorranceMaterialData()
    {
        return static_cast<void*>(&albedoVector4);
    }

    static inline size_t GetOpaqueCookTorranceMaterialDataSize()
    {
        return 2 * sizeof(Math::Vector4);
    }

    void operator=(const CookTorranceMaterial& material)
    {
        albedo    = material.albedo;
        roughness = material.roughness;
        fresnel   = material.fresnel;
    }

    union
    {
        struct
        {
            Math::Vector3 albedo; UINT useDiffuseMap;
        };
        Math::Vector4 albedoVector4;
    };
    float roughness;
    float metallic;
    float fresnel;
    float useSpecularMap; // Vector4
};

union MaterialMap
{
    struct MaterialMapStruct
    {
        Texture2D* diffuseMap;
        Texture2D* specMap;
        Texture2D* normalMap;
        Texture2D* lightMap;
    }m_materialMapStruct;

    Texture2D* m_materialMapTextures[4];
};