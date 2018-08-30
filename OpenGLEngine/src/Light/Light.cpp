#include "Light.h"

Light::Light()
    : m_light_vec4(Math::Vector3(), LightType::UNKNOWN_LIGHT)
{
}

Light::Light(
    const LightType lightType,
    const Math::Vector3& color)
    : m_light_vec4(color, static_cast<float>(lightType))
{
}

Light::~Light()
{
}
