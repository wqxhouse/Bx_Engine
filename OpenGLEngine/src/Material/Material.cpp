#include "Material.h"

Material::Material(float refraction, float alpha, bool transparency)
{
}

void Material::setShadingMethod(const ShadingMethod shadingMethod)
{
}

SpecularMaterial::SpecularMaterial(const Vector3 & ks, const Vector3 & kd, float refraction, float alpha, bool transparency)
	:Material(refraction, alpha, transparency)
{
}
