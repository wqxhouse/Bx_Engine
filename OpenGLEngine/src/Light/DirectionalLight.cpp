#include "Light.h"

DirectionalLight::DirectionalLight(const glm::vec3 & direction, const glm::vec3 & color)
	:Light(color)
{
	m_direction = glm::normalize(direction);
}

DirectionalLight::~DirectionalLight()
{
}
