#pragma once

#include "../Math/Math.h"
#include "../Shadow/ShadowMap.h"

enum LightType
{
    DIRECTIONAL_LIGHT = 0x0,
    POINT_LIGHT       = 0x1,
    SPOT_LIGHT        = 0x2,
    AREA_LIGHT        = 0x3,
    UNKNOWN_LIGHT     = 0xffffffff
};

class Light
{
public:
    Light();
	Light(const LightType lightType, const Math::Vector3& color);
	~Light();

    virtual void translate(const Math::Vector3& transVector) {}
    virtual void rotate(const Math::Vector3& axis, const float angle) {}

    inline LightType     GetLightType()      const { return m_lightType;                              }
    inline Math::Vector3 GetLightColor()     const { return m_color;                                  }
    inline Math::Vector4 GetLightColorVec4() const { return m_light_vec4;                             }
    inline void*         GetDataPtr()              { return reinterpret_cast<void*>(&m_light_vec4);   }

    inline BOOL          IsLightEnable()     const { return ((enableLight == TRUE_F) ? TRUE : FALSE); }
    inline void          EnableLight()             { enableLight = TRUE_F;                            }
    inline void          DisableLight()            { enableLight = FALSE_F;                           }

/// Light data1
protected:
    float     reserve;     // Padding

    LightType m_lightType; // Light type
    BOOL_F    enableLight; // Check if light enabled

/// Light data2
private:
    union
    {
        struct
        {
            Math::Vector3 m_color;      // Light color
            ShadowMap*    m_pShadowMap; // Shadow map pointer
        };
        Math::Vector4 m_light_vec4;
    };
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(const Math::Vector3& direction, const Math::Vector3& color);
	~DirectionalLight();

    void rotate(const Math::Vector3& axis, const float angle);

    static inline UINT GetDataSize() { return (sizeof(DirectionalLight));  }

	inline Math::Vector3 GetDir()     const { return m_direction;      }
    inline Math::Vector4 GetDirVec4() const { return m_direction_vec4; }

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
    Math::Vector4 padding;
};

class PointLight : public Light
{
public:
	PointLight(const Math::Vector3& position, const Math::Vector3& color, float radius);
	~PointLight();

    void translate(const Math::Vector3& transVector);

    static inline UINT GetDataSize() { return (sizeof(PointLight)); }

	inline Math::Vector3 GetPos()      const { return m_position;    }

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
    Math::Vector4 padding;
};

class SpotLight : public Light
{
public:
	SpotLight(
        const Math::Vector3& position,
        const Math::Vector3& direction,
        const Math::Vector3& color,
        float distance,
        float in_angle,
        float out_angle);

	~SpotLight();

    void translate(const Math::Vector3& transVector);
    void rotate(const Math::Vector3& axis, const float angle);

    static inline UINT GetDataSize() { return (sizeof(SpotLight)); }

private:
    union
    {
        Math::Vector4 m_v4[2];

        struct
        {
            Math::Vector3 m_position;  // Light position
            float m_in_cosTheta;

            Math::Vector3 m_direction; // Light direction
            float m_out_cosTheta;
        };
    };

public:
	inline Math::Vector3  GetPos()           const { return m_position;      }
    inline Math::Vector3  GetDirection()     const { return m_direction;     }
    inline Math::Vector4* GetSpotLightData()       { return &(m_v4[0]);      }

    inline float GetInnerAngle() const { return std::acos(m_in_cosTheta); }
    inline float GetOuterAngle() const { return std::acos(m_out_cosTheta); }
};

//TODO: other kinds of lights