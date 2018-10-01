#pragma once

#include "../Shader/GraphicsShader.h"
#include "../Shader/ComputeShader.h"
#include "../Math/Vector4.h"

class Scene;

class ForwardPlusRender
{
public:
    ForwardPlusRender(Scene* pScene);
    ~ForwardPlusRender();

    BOOL initialize();
    void update();
    void draw();

private:
    Scene* m_pScene;

    ComputeShader  m_gridFrustumComputeShader;
    GraphicsShader m_renderShader;

    GLuint m_frustumVerticesSsbo;

    Resolution m_resolution;
    UINT       m_frustumNum[2];
    UINT       m_frustumSize[2];

    struct SimpleFrustum
    {
        Plane planes[4];
    };

    std::vector<SimpleFrustum> m_frustums;
};