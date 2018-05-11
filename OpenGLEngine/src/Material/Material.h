#pragma once

#include "../Math/Vector4.h"

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

struct SimpleSpecularMaterial
{
    Vector4 ka;
    Vector4 ks;
    Vector4 kd;
    Vector4 ns;
};

class Material
{
public:
	Material(const std::string& materialName, float refraction, float alpha, bool transparency);

	void setShadingMethod(const ShadingMethod shadingMethod);
	virtual ~Material() {}

	std::string m_materialName;

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

	Vector3 ka;
	Vector3 ks;
	Vector3 kd;
	float ns;
};