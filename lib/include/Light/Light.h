#pragma once

#include "../Core/OpenGLPCH.h"
#include "../Math/Math.h"

enum LightType
{
    DIRECTIONAL_LIGHT = 0,
    POINT_LIGHT       = 1,
    SPOT_LIGHT        = 2,
};

class Light
{
public:
	Light(const LightType lightType, const Math::Vector3& color);
	~Light();

    //inline LightType     GetLightType()      const { return m_lightType;                            }
    inline Math::Vector3 GetLightColor()     const { return m_color;                                }
    inline Math::Vector4 GetLightColorVec4() const { return m_color_vec4;                           }
    inline void*         GetDataPtr()              { return reinterpret_cast<void*>(&m_color_vec4); }

    LightType m_lightType;

protected:
    union
    {
        struct
        {
            Math::Vector3 m_color; // Light color
            float colorPadding;
        };
        Math::Vector4 m_color_vec4;
    };

private:
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(const Math::Vector3& direction, const Math::Vector3& color);
	~DirectionalLight();

    void rotate(const Math::Vector3& axis, const float angle);

    inline UINT GetDataSize()         const { return (sizeof(*this) - sizeof(LightType)); }
	inline Math::Vector3 GetDir()     const { return m_direction;                         }
    inline Math::Vector4 GetDirVec4() const { return m_direction_vec4;                    }

private:
    union
    {
        struct
        {
            Math::Vector3 m_direction; // Light direction
            float directionPadding;
        };
        Math::Vector4 m_direction_vec4;
    };
};

class PointLight : public Light
{
public:
	PointLight(const Math::Vector3& position, const Math::Vector3& color, float radius);
	~PointLight();

    inline UINT          GetDataSize() const { return (sizeof(*this) - sizeof(LightType)); }
	inline Math::Vector3 GetPos()      const { return m_position; }

private:
    union
    {
        struct
        {
            Math::Vector3 m_position; // Light position
            float         m_radius;   // Light radius
        };
        Math::Vector4 m_pointLightData;
    };
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

    inline UINT GetDataSize()     const { return (sizeof(*this) - sizeof(LightType)); }
	inline Math::Vector3 GetPos() const { return m_position; }

private:
	Math::Vector3 m_position; // Light position
	float m_radius_in;
	float m_radius_out;
};

//TODO: other kinds of lights