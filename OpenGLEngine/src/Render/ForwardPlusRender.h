#pragma once

#include "../Shader/GraphicsShader.h"
#include "../Shader/ComputeShader.h"
#include "../Math/Vector4.h"

class ForwardPlusRender
{
public:
    ForwardPlusRender();
    ~ForwardPlusRender();

    BOOL initialize();
    void update();
    void draw();

private:
    ComputeShader  m_gridFrustumComputeShader;
    GraphicsShader m_renderShader;

    GLuint m_frustumVerticesSsbo;

    Math::Vector4 m_frustums[4];
};