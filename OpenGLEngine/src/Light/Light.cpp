#include "Light.h"

Light::Light(
    const LightType lightType,
    const Math::Vector3& color)
    : m_lightType(lightType), m_color(color)
{
}

Light::~Light()
{
}
