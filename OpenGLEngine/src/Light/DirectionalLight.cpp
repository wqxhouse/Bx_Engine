#include "Light.h"

DirectionalLight::DirectionalLight(
    const Math::Vector3& direction,
    const Math::Vector3& color)
	:Light(DIRECTIONAL_LIGHT, color)
{
	m_direction = Math::Vector3::Normalize(direction);
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::rotate(
    const Math::Vector3& axis,
    const float angle)
{
    m_direction = Math::rotate(m_direction, axis, angle);
}
