//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../../Core/PCH.h"
#include "../Math.h"

// Translation class
class Trans
{
public:
    Trans(
        const Math::Vector3& pos,
        const Math::Vector3& center,
        const Math::Vector3& up)
        : scale(1.0f, 1.0f, 1.0f),
          pitch(0.0f), yaw(0.0f), roll(0.0f)
	{
		this->pos = pos;
		this->front = Math::Vector3::Normalize(center - pos);
		this->up    = Math::Vector3::Normalize(up);
		this->right = Math::Vector3::Normalize(Math::Vector3::crossProduct(front, up));

        initialize();

        m_transFlags.flags = 0;
	}

    Trans(
        const Math::Vector3& pos,
        const Math::Vector3& center,
        const Math::Vector3& up,
        const Math::Vector3& scale,
        const float          pitch,
        const float          yaw,
        const float          roll)
    {
        this->pos   = pos;
        this->front = Math::Vector3::Normalize(center - pos);
        this->up    = Math::Vector3::Normalize(up);
        this->right = Math::Vector3::Normalize(Math::Vector3::crossProduct(front, up));

        this->scale = scale;
        this->pitch = pitch;
        this->yaw   = yaw;
        this->roll  = roll;

        initialize();

        m_transFlags.flags = 0;
    }

    void update();

    INLINE const Math::Vector3& GetPos() const
    {
        return pos;
    }

    INLINE Math::Vector3 GetFront() const
    {
        return front;
    }

    INLINE Math::Vector3 GetRight() const
    {
        return right;
    }

    INLINE Math::Vector3 GetUp() const
    {
        return up;
    }

	INLINE const Math::Mat4& GetViewMat()
	{
        if (m_transFlags.bits.viewFlag == 1)
        {
            viewMatrix = Math::viewMatrix(pos, front, up);
            m_transFlags.bits.viewFlag = 0;
        }
		return viewMatrix;
	}

    INLINE const Math::Mat4& GetTranslateMatrix()
    {
        if (m_transFlags.bits.posFlag == 1)
        {
            assert(m_transFlags.bits.viewFlag == 1);

            translationMatrix = Math::Translate(Math::Mat4::Identity(), pos);
            m_transFlags.bits.posFlag = 0;
        }

        return translationMatrix;
    }

    INLINE const Math::Mat4& GetScaleMatrix()
    {
        if (m_transFlags.bits.scaleFlag == 1)
        {
            scaleMatrix = Math::Scale(Math::Mat4::Identity(), scale);
            m_transFlags.bits.scaleFlag = 0;
        }

        return scaleMatrix;
    }

    Math::Mat4 GetTransMatrix();

    INLINE void TransPos(const Math::Vector3& trans)
    {
        this->pos += trans;

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.posFlag   = 1;
        m_transFlags.bits.viewFlag  = 1;
    }

    INLINE void SetTrans(
        const Math::Vector3& pos,
        const Math::Vector3& center,
        const Math::Vector3& up)
    {
        this->pos   = pos;
        this->front = Math::Vector3::Vector3::Normalize(center - pos);
        this->up    = Math::Vector3::Normalize(up);
        this->right = Math::Vector3::Normalize(Math::Vector3::crossProduct(front, up));

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.posFlag   = 1;
        m_transFlags.bits.viewFlag  = 1;
    }

    INLINE void SetTransBase(
        const Math::Vector3& front,
        const Math::Vector3& right)
    {
        this->front = front;
        this->right = right;
        this->up    = Math::Vector3::Normalize(Math::Vector3::crossProduct(right, front));

        m_transFlags.bits.viewFlag = 1;
    }

    INLINE void SetScale(const Math::Vector3& scale)
    {
        this->scale = scale;

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.scaleFlag = 1;
    }

    INLINE void SetRotation(const float pitch, const float yaw, const float roll)
    {
        this->pitch = pitch;
        this->yaw   = yaw;
        this->roll  = roll;

        m_transFlags.bits.transFlag = 1;
        m_transFlags.bits.rotationFlag = 1;
    }

private:
    void initialize();

    // Trans members
    Math::Vector3 front;
    Math::Vector3 up;
    Math::Vector3 right;

    // Translation
    Math::Vector3 pos;

    // Scale
    Math::Vector3 scale;

    // Rotation
    float pitch;
    float yaw;
    float roll;

    // Matrix
    Math::Mat4 transMatrix;

    Math::Mat4 translationMatrix;
    Math::Mat4 scaleMatrix;
    Math::Mat4 rotationMatrix;

    Math::Mat4 viewMatrix;

    // Flags
    union TransFlags
    {
        UINT flags;

        struct
        {
            UINT transFlag    : 1;
            UINT posFlag      : 1;
            UINT scaleFlag    : 1;
            UINT rotationFlag : 1;
            UINT viewFlag     : 1;
            UINT reserve      : 27;
        } bits;
    }m_transFlags;
};