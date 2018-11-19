#pragma once

#include "../Core/PCH.h"
#include "../Math/Math.h"

namespace Object
{
    namespace Light
    {
        class LightBase
        {
        public:
            LightBase();
            LightBase(const LightType lightType, const Math::Vector3& color);
            ~LightBase();

            virtual void translate(const Math::Vector3& transVector) {}
            virtual void rotate(const Math::Vector3& axis, const float angle) {}

            virtual inline Math::Vector3 GetDir() { return Math::Vector3(); }

            inline LightType     GetLightType()      const { return m_lightType; }
            inline Math::Vector3 GetLightColor()     const { return m_color; }
            inline Math::Vector4 GetLightColorVec4() const { return m_light_vec4; }
            inline void*         GetDataPtr() { return reinterpret_cast<void*>(&m_light_vec4); }

            inline BOOL          IsLightEnable()     const { return ((enableLight == TRUE_F) ? TRUE : FALSE); }
            inline void          EnableLight() { enableLight = TRUE_F; }
            inline void          DisableLight() { enableLight = FALSE_F; }

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
            DirectionalLight(const Math::Vector3& direction, const Math::Vector3& color);
            ~DirectionalLight();

            void rotate(const Math::Vector3& axis, const float angle);

            static inline UINT GetDataSize() { return (sizeof(DirectionalLight)); }

            inline Math::Vector3 GetDir()     const { return m_direction; }
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

        class PointLight : public LightBase
        {
        public:
            PointLight(const Math::Vector3& position, const Math::Vector3& color, float radius);
            ~PointLight();

            void translate(const Math::Vector3& transVector);

            static inline UINT GetDataSize() { return (sizeof(PointLight)); }

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
            Math::Vector4 padding;
        };

        class SpotLight : public LightBase
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
            inline Math::Vector3  GetPos()           const { return m_position; }
            inline Math::Vector3  GetDir()           const { return m_direction; }
            inline Math::Vector4* GetSpotLightData() { return &(m_v4[0]); }

            inline float GetInnerAngleRadians() const { return std::acos(m_in_cosTheta)  * 2.0f; }
            inline float GetOuterAngleRadians() const { return std::acos(m_out_cosTheta) * 2.0f; }

            inline float GetInnerAngleDegree() const { return Math::Degree(GetInnerAngleRadians()); }
            inline float GetOuterAngleDegree() const { return Math::Degree(GetOuterAngleRadians()); }
        };

        //TODO: other kinds of lights
    }
}