//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Material.h"

namespace Object
{
    namespace Model
    {
        Material::Material(
            const MaterialType materialType,
            const std::string& materialName,
            const float        refraction,
            const float        alpha,
            const bool         transparency)
            : m_materialType(materialType),
              m_materialName(materialName),
              m_shadingMethod(ShadingMethod::Phong)
        {
            this->refraction   = refraction;
            this->alpha        = alpha;
            this->transparency = transparency;
        }

        void Material::setShadingMethod(
            const ShadingMethod shadingMethod)
        {
        }

        PhongMaterial::PhongMaterial(
            const std::string & materialName)
            : Material(PHONG, materialName, 1.0f, 1.0f, false),
            m_specularMaterialData(0.0f)
        {
        }

        PhongMaterial::PhongMaterial(
            const std::string&   materialName,
            const Math::Vector3& ka,
            const Math::Vector3& kd,
            const Math::Vector3& ks,
            float ns)
            : Material(PHONG, materialName, 1.0f, 1.0f, false),
            m_specularMaterialData(0.0f)
        {
            this->ka = ka;
            this->kd = kd;
            this->ks = ks;
            this->ns = ns;
        }

        PhongMaterial::PhongMaterial(
            const std::string&    materialName,
            const Math::Vector3&  ka,
            const Math::Vector3&  kd,
            const Math::Vector3 & ks,
            float                 ns,
            float                 refraction,
            float                 alpha,
            bool                  transparency)
            : Material(PHONG, materialName, refraction, alpha, transparency)
        {
            this->ka = ka;
            this->kd = kd;
            this->ks = ks;
            this->ns = ns;
        }
    }
}