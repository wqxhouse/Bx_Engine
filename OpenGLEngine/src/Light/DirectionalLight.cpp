#include "Light.h"

DirectionalLight::DirectionalLight(const glm::vec3 & direction, const glm::vec3 & color)
	:Light(DIRECTIONAL_LIGHT, color)
{
	m_direction = glm::normalize(direction);
}

DirectionalLight::~DirectionalLight()
{
}
