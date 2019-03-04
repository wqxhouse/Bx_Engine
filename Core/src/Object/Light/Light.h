//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../../Math/Math.h"
#include "../ObjectBase.h"

#define MAX_DIRECTIONAL_LIGHT_NUM   16
#define MAX_DYNAMIC_POINT_LIGHT_NUM 16
#define MAX_DYNAMIC_SPOT_LIGHT_NUM  16
#define MAX_STATIC_POINT_LIGHT_NUM  8192
#define MAX_STATIC_SPOT_LIGHT_NUM   8192

#define MAX_DYNAMIC_LIGHT_NUM       1024
#define MAX_STATIC_LIGHT_NUM        16384

namespace Object
{
    namespace Light
    {
        struct LightCreateInfo
        {
            Trans*        pTrans;
            Math::Vector3 lightColor;
        };

        struct PointLightCreateInfo : public LightCreateInfo
        {
            float radius;
        };

        struct SpotLightCreateInfo : public LightCreateInfo
        {
            float distance;
            float in_angle;
            float out_angle;
        };

        class LightBase : public ObjectBase
        {
        public:
            LightBase(
                const UINT             objectId,
                const LightType        lightType,
                const LightCreateInfo& lightCreateInfo);

            virtual ~LightBase();

            virtual void translate(const Math::Vector3& transVector) {}
            virtual void rotate(const Math::Vector3& axis, const float angle) {}

            virtual INLINE Math::Vector3 GetDir() { return Math::Vector3(); }

            INLINE LightType     GetLightType()      const { return m_lightType; }
            INLINE Math::Vector3 GetLightColor()     const { return m_color; }
            INLINE Math::Vector4 GetLightColorVec4() const { return m_light_vec4; }
            INLINE void*         GetDataPtr() { return reinterpret_cast<void*>(&m_light_vec4); }

            INLINE BOOL          IsLightEnable()     const { return ((enableLight == TRUE_F) ? TRUE : FALSE); }
            INLINE void          EnableLight() { enableLight = TRUE_F; }
            INLINE void          DisableLight() { enableLight = FALSE_F; }

        protected:
            /// Light data1
            ///{
            float     reserve;     // Padding

            LightType m_lightType; // Light type
            BOOL_F    enableLight; // Check if light enabled
        ///}

        private:
            /// Light data2
            ///{
            union
            {
                struct
                {
                    Math::Vector3 m_color;      // Light color
                    float         padding;
                };
                Math::Vector4 m_light_vec4;
            };
            ///}

            /// Light data3
            ///{
            Math::Mat4 m_lightTransVP; // Light VP trans matrix for shadow casting
        ///}
        };

        class DirectionalLight : public LightBase
        {
        public:
            DirectionalLight() : LightBase(INVALID_OBJECT_ID, DIRECTIONAL_LIGHT, {}) {}

            DirectionalLight(
                const UINT             objectId,
                const LightCreateInfo& lightCreateInfo);

            ~DirectionalLight();

            void rotate(const Math::Vector3& axis, const float angle);

            static INLINE UINT GetDataSize() { return (sizeof(DirectionalLight)); }

            INLINE Math::Vector3 GetDir()     const { return m_direction; }
            INLINE Math::Vector4 GetDirVec4() const { return m_direction_vec4; }

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

        class PointLight : public LightBase
        {
        public:
            PointLight() : LightBase(INVALID_OBJECT_ID, POINT_LIGHT, {}) {}

            PointLight(
                const UINT                  objectId,
                const PointLightCreateInfo& pointLightCreateInfo);

            ~PointLight();

            void translate(const Math::Vector3& transVector);

            static INLINE UINT GetDataSize()          { return (sizeof(PointLight)); }

            INLINE const Math::Vector3 GetPos() const { return m_position; }

            INLINE const float GetRadius()      const { return m_radius; }

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

        class SpotLight : public LightBase
        {
        public:
            SpotLight() : LightBase(INVALID_OBJECT_ID, SPOT_LIGHT, {}) {}

            SpotLight(
                const UINT                 objectId,
                const SpotLightCreateInfo& spotLightCreateInfo);

            ~SpotLight();

            void translate(const Math::Vector3& transVector);
            void rotate(const Math::Vector3& axis, const float angle);

            static INLINE UINT GetDataSize() { return (sizeof(SpotLight)); }

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
            INLINE Math::Vector3  GetPos()           const { return m_position;  }
            INLINE Math::Vector3  GetDir()           const { return m_direction; }
            INLINE Math::Vector4* GetSpotLightData()       { return &(m_v4[0]);  }

            INLINE const float GetInnerAngleCosine() const { return m_in_cosTheta;  }
            INLINE const float GetOuterAngleCosine() const { return m_out_cosTheta; }

            INLINE float GetInnerAngleRadians() const { return std::acos(m_in_cosTheta)  * 2.0f; }
            INLINE float GetOuterAngleRadians() const { return std::acos(m_out_cosTheta) * 2.0f; }

            INLINE float GetInnerAngleDegree() const { return Math::Degree(GetInnerAngleRadians()); }
            INLINE float GetOuterAngleDegree() const { return Math::Degree(GetOuterAngleRadians()); }
        };

        //TODO: other kinds of lights
    }
}