#include "Light.h"

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float radius)
	:Light(POINT_LIGHT, color)
{
	m_position = position;
	m_radius = radius;
}

PointLight::~PointLight()
{

}