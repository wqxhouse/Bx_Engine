#pragma once

#include "../Math/Matrix4x4.h"

using namespace Math;

enum MaterialType
{
	SPECULAR, ALBEDO, PBR
};

enum ShadingMethod
{
	Flat,
	Gouraud,
	Phong,
	Blin_Phong,
	PBR_LAMBERT,
	PBR_COOK_TORRANCE
};

class Material
{
public:
	Material(const std::string& materialName, float refraction, float alpha, bool transparency);

	void setShadingMethod(const ShadingMethod shadingMethod);
	virtual ~Material() {}

	std::string m_materialName;
    //UINT        m_materialId;

	MaterialType materialType;
	ShadingMethod shadingMethod;

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

    union
    {
        struct
        {
            Vector3 ka; float ka_padding;
            Vector3 kd; float kd_padding;
            Vector3 ks; float ns; // Vector4
        };
        Mat4 m_materialData;
    };
};