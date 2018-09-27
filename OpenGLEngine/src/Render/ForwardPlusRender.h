#pragma once

#include "../Shader/Shader.h"
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
    Shader m_gridFrustumComputeShader;
    Shader m_renderShader;

    GLuint m_frustumVerticesSsbo;

    Math::Vector4 m_frustums[4];
};