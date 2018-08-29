#include "Light.h"

Light::Light(
    const LightType lightType,
    const Math::Vector3& color)
    : m_light_vec4(color, static_cast<float>(lightType))
{
}

Light::~Light()
{
}
