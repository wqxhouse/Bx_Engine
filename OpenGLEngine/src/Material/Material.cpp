#include "Material.h"

Material::Material(const std::string& materialName, float refraction, float alpha, bool transparency)
{
}

void Material::setShadingMethod(const ShadingMethod shadingMethod)
{
}

SpecularMaterial::SpecularMaterial(const std::string & materialName)
	: Material(materialName, 1.0f, 1.0f, false)
{
}

SpecularMaterial::SpecularMaterial(const std::string & materialName,
	                               const Vector3 & ka, const Vector3 & kd, const Vector3 & ks, float ns)
	: Material(materialName, 1.0f, 1.0f, false)
{
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->ns = ns;
}

SpecularMaterial::SpecularMaterial(const std::string & materialName,
	                               const Vector3 & ka, const Vector3 & kd, const Vector3 & ks, float ns,
	                               float refraction, float alpha, bool transparency)
	: Material(materialName, refraction, alpha, transparency)
{
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->ns = ns;
}
