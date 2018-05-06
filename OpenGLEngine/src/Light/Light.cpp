#include "Light.h"

Light::Light(const LightType lightType, const glm::vec3& color)
    : m_lightType(lightType), m_color(color)
{
}

Light::~Light()
{
}
