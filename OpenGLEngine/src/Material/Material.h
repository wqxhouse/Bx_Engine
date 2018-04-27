#pragma once

#include "../Math/Vector3.h"

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
	Material(float refraction, float alpha, bool transparency);

	void setShadingMethod(const ShadingMethod shadingMethod);
	virtual ~Material() {}

	MaterialType materialType;
	ShadingMethod shadingMethod;

	bool transparency;
	float refraction;
	float alpha;
};

class SpecularMaterial : public Material
{
public:
	SpecularMaterial(const Vector3& ks, const Vector3& kd, float refraction,
		             float alpha, bool transparency);

	~SpecularMaterial() {}

	Vector3 ks;
	Vector3 kd;
};