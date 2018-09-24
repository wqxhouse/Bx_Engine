#pragma once

#include "../Shader/Shader.h"

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
};