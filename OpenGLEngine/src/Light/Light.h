#pragma once

#include "../Core/OpenGLPCH.h"

class Light
{
public:
	Light(const glm::vec3& color);
	~Light();

	inline glm::vec3 getLightColor() { return m_color; }

	enum LightType
	{
		DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT
	}m_lightType;

private:
	glm::vec3 m_color; // Light color
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(const glm::vec3& direction, const glm::vec3& color);
	~DirectionalLight();

	inline glm::vec3 getDir() { return m_direction;  }
private:
	glm::vec3 m_direction; // Light direction
};

class PointLight : public Light
{
public:
	PointLight(const glm::vec3& position, const glm::vec3& color, float radius);
	~PointLight();

	inline glm::vec3 getPos() { return m_position; }
private:
	glm::vec3 m_position; // Light position
	float m_radius;
};

class SpotLight : public Light
{
public:
	SpotLight(const glm::vec3 position, const glm::vec3& color, float radius_in, float raidus_out);
	~SpotLight();

	inline glm::vec3 getPos() { return m_position; }
private:
	glm::vec3 m_position; // Light position
	float m_radius_in;
	float m_radius_out;
};

//TODO: other kinds of lights