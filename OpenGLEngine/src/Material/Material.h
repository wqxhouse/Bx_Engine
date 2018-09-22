#pragma once

#include "../Math/Matrix4x4.h"
#include "../Core/TypeDef.h"

using namespace Math;

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
		             const Vector3& ka, const Vector3& kd, const Vector3& ks, float ns);
	SpecularMaterial(const std::string& materialName,
		             const Vector3& ka, const Vector3& kd, const Vector3& ks,
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
            Vector3 ka; float ka_padding;
            Vector3 kd; float kd_padding;
            Vector3 ks; float ns; // Vector4
            Vector4 padding;
        };
        Mat4 m_specularMaterialData;
    };
};

class CookTorranceMaterial : public Material
{
public:
    // TODO: Load material from file
    CookTorranceMaterial()
        : Material(COOKTORRANCE, "", 1.0f, 1.0f, false),
          albedoVector4(Vector4(1.0f)),
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
        return 2 * sizeof(Vector4);
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
            Vector3 albedo; float albedoPadding;
        };
        Vector4 albedoVector4;
    };
    float roughness;
    float metallic;
    float fresnel;
    float padding; // Vector4
};