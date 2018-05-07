#include "Light.h"

DirectionalLight::DirectionalLight(const glm::vec3 & direction, const glm::vec3 & color)
	:Light(DIRECTIONAL_LIGHT, color)
{
	m_direction = glm::normalize(direction);
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::rotate(const const Math::Vector3& axis, const float angle)
{
    Math::Vector3 v3 = Math::rotate(Math::Vector3(m_direction.x, m_direction.y, m_direction.z), axis, angle);
    m_direction.x = v3.x;
    m_direction.y = v3.y;
    m_direction.z = v3.z;
}
