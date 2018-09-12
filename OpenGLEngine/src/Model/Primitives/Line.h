#pragma once

#include "Primitive.h"
#include "../../Math/Matrix4x4.h"
#include "../../Shader/Shader.h"

class Line
{
public:
    Line(const Math::Vector3& p1, const Math::Vector3& p2);
    ~Line();

    BOOL initialize();
    void draw();

    inline void SetLineData(const Math::Vector3& p1, const Math::Vector3& p2)
    {
        m_lineData[0] = p1;
        m_lineData[1] = p2;
    }

    inline void SetTransform(const Math::Mat4& trans) { m_trans = trans; }

    inline void SetLineColor(const Math::Vector3& color) { m_lineColor = color; }
    inline Math::Vector3 GetLineColor() const { return m_lineColor; }

private:
    Shader m_lineShader;

    GLuint m_vertexArrayObj;
    GLuint m_vertexBufObj;

    Math::Vector3 m_lineData[2];

    Math::Vector3 m_lineColor;

    Math::Mat4 m_trans;
};