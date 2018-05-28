#pragma once

#include "../../Core/OpenGLPCH.h"
#include "../Quaternion.h"

// Translation class
class Trans
{
public:
    Trans(glm::vec3 pos, glm::vec3 center, glm::vec3 up)
        : scale(1.0f, 1.0f, 1.0f),
          pitch(0.0f), yaw(0.0f), roll(0.0f)
	{
		this->pos = pos;
		this->front = glm::normalize(center - pos);
		this->up = glm::normalize(up);
		this->right = glm::normalize(glm::cross(front, up));

        m_transFlags.flags = 0;

        initialize();
	}

    Trans(glm::vec3 pos, glm::vec3 center, glm::vec3 up,
        glm::vec3 scale,
        float pitch, float yaw, float roll)
    {
        this->pos = pos;
        this->front = glm::normalize(center - pos);
        this->up = glm::normalize(up);
        this->right = glm::normalize(glm::cross(front, up));

        this->scale = scale;
        this->pitch = pitch;
        this->yaw   = yaw;
        this->roll  = roll;

        m_transFlags.flags = 0;

        initialize();
    }

    void update();

    inline glm::vec3 GetPos() const
    {
        return pos;
    }

	inline glm::mat4 GetViewMat() const
	{
		return glm::lookAt(pos, pos + front, up);
	}

    inline glm::mat4 GetTranslateMatrix()
    {
        if (m_transFlags.bits.posFlag == 1)
        {
            translationMatrix = glm::translate(glm::mat4(), pos);
            m_transFlags.bits.posFlag = 0;
        }

        return translationMatrix;
    }

    inline glm::mat4 GetScaleMatrix()
    {
        if (m_transFlags.bits.scaleFlag == 1)
        {
            scaleMatrix = glm::scale(glm::mat4(), scale);
            m_transFlags.bits.scaleFlag = 0;
        }

        return scaleMatrix;
    }

    glm::mat4 GetTransMatrix();

    inline void TransPos(const glm::vec3 trans)
    {
        this->pos += trans;

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.posFlag = 1;
    }

    inline void SetScale(const glm::vec3 scale)
    {
        this->scale = scale;

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.scaleFlag = 1;
    }

    inline void SetRotation(const float pitch, const float yaw, const float roll)
    {
        this->pitch = pitch;
        this->yaw   = yaw;
        this->roll  = roll;

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.rotationFlag = 1;
    }

    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

private:
    void initialize();

    // Translation
    glm::vec3 pos;

    // Scale
    glm::vec3 scale;

    // Rotation
    float pitch;
    float yaw;
    float roll;

    // Matrix
    glm::mat4 transMatrix;

    glm::mat4 translationMatrix;
    glm::mat4 scaleMatrix;
    glm::mat4 rotationMatrix;

    glm::mat4 viewMatrix;

    // Flags
    union TransFlags
    {
        UINT flags;

        struct
        {
        public:
            UINT transFlag    : 1;
            UINT posFlag      : 1;
            UINT scaleFlag    : 1;
            UINT rotationFlag : 1;
            UINT viewFlag     : 1;
        private:
            UINT reserve      : 29;
        }bits;
    }m_transFlags;
};