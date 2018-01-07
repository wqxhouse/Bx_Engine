#include "Light.h"

DirectionalLight::DirectionalLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color)
	:Light(position, direction, color)
{

}

DirectionalLight::~DirectionalLight()
{
}
