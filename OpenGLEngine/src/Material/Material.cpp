#include "Material.h"

Material::Material(
    const MaterialType materialType,
    const std::string& materialName,
    const float refraction,
    const float alpha,
    const bool transparency)
    : m_materialType(materialType),
      m_materialName(materialName)      
{
    this->refraction   = refraction;
    this->alpha        = alpha;
    this->transparency = transparency;
}

void Material::setShadingMethod(const ShadingMethod shadingMethod)
{
}

SpecularMaterial::SpecularMaterial(const std::string & materialName)
	: Material(PHONG, materialName, 1.0f, 1.0f, false),
      m_specularMaterialData(0.0f)
{
}

SpecularMaterial::SpecularMaterial(
    const std::string& materialName,
	const Vector3&     ka,
    const Vector3&     kd,
    const Vector3&     ks,
    float ns)
	: Material(PHONG, materialName, 1.0f, 1.0f, false),
      m_specularMaterialData(0.0f)
{
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->ns = ns;
}

SpecularMaterial::SpecularMaterial(
    const std::string& materialName,
    const Vector3& ka,
    const Vector3& kd,
    const Vector3 & ks,
    float ns,
	float refraction,
    float alpha,
    bool transparency)
	: Material(PHONG, materialName, refraction, alpha, transparency)
{
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;
	this->ns = ns;
}
