#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Math/Math.h"

enum LightType
{
    DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT
};

class Light
{
public:
	Light(const LightType lightType, const Math::Vector3& color);
	~Light();

	inline Math::Vector3 getLightColor() const { return m_color; }

    LightType m_lightType;

private:
    Math::Vector3 m_color; // Light color
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(const Math::Vector3& direction, const Math::Vector3& color);
	~DirectionalLight();

    void rotate(const Math::Vector3& axis, const float angle);
	inline Math::Vector3 getDir() { return m_direction;  }
private:
    Math::Vector3 m_direction; // Light direction
};

class PointLight : public Light
{
public:
	PointLight(const Math::Vector3& position, const Math::Vector3& color, float radius);
	~PointLight();

	inline Math::Vector3 getPos() { return m_position; }
private:
    Math::Vector3 m_position; // Light position
	float m_radius;
};

class SpotLight : public Light
{
public:
	SpotLight(
        const Math::Vector3& position,
        const Math::Vector3& color,
        float radius_in,
        float raidus_out);

	~SpotLight();

	inline Math::Vector3 getPos() const { return m_position; }
private:
	Math::Vector3 m_position; // Light position
	float m_radius_in;
	float m_radius_out;
};

//TODO: other kinds of lights