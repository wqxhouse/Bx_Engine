#include "ForwardPlusRender.h"

#include "../Context/Scene.h"

#define DEFAULT_FRUSTUM_SIZE_X 16
#define DEFAULT_FRUSTUM_SIZE_Y 16

using Math::Vector4;
using Math::Mat4;

ForwardPlusRender::ForwardPlusRender(
    Scene* pScene)
    : m_pScene(pScene)
{
    m_resolution = m_pScene->GetSetting()->resolution;

    m_frustumSize[0] = DEFAULT_FRUSTUM_SIZE_X;
    m_frustumSize[1] = DEFAULT_FRUSTUM_SIZE_Y;

    m_frustumNum[0] =
        static_cast<UINT>(std::ceil(static_cast<float>(m_resolution.width) / static_cast<float>(m_frustumSize[0])));
    m_frustumNum[1] =
        static_cast<UINT>(std::ceil(static_cast<float>(m_resolution.height) / static_cast<float>(m_frustumSize[1])));

    m_frustums.resize(m_frustumNum[0] * m_frustumNum[1]);

    m_gridFrustumBindingPoint                     = 0;
    m_tiledLightList.m_lightGridBindingPoint      = 1;
    m_tiledLightList.m_lightIndexListBindingPoint = 2;

    m_renderFlags.value = 0;
}

ForwardPlusRender::~ForwardPlusRender()
{
}

BOOL ForwardPlusRender::initialize()
{
    BOOL result = TRUE;

    assert(result == TRUE);

    if (result == TRUE)
    {
        result = initGridFrustums();
    }

    assert(result == TRUE);

    return result;
}

void ForwardPlusRender::update()
{
    BOOL result = initTileLightList();

    assert(result == TRUE);
}

void ForwardPlusRender::draw()
{
    calGridFrustums();

    lightCulling();

    // Sample of mapping SSBO
    SsboMgr* pSsboMgr = m_pScene->GetSsboMgr();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pSsboMgr->GetSsbo(2)->GetSsboHandle());
    UINT* pData = static_cast<UINT*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    /*for (int i = 0; i < 4; ++i)
    {
        gpuFrustumsPtr[i] = m_tests[i];
    }*/

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

BOOL ForwardPlusRender::initGridFrustums()
{
    BOOL result = TRUE;

    m_gridFrustumComputeShader.setShader("GridFrustumsCompute.cs");
    result = m_gridFrustumComputeShader.compileShaderProgram();

    assert(result == TRUE);

    m_gridFrustumComputeShader.setTotalThreadSize(
        m_frustumNum[0], // Number of frustum tiles on X
        m_frustumNum[1], // Number of frustum tiles on Y
        1);              // Keep Z to 1

    m_gridFrustumComputeShader.setThreadGroupSize(m_frustumSize[0], m_frustumSize[1], 1);
    m_gridFrustumComputeShader.calGroupNum();

    // Generate frustum SSBO
    m_pScene->GetSsboMgr()->addStaticSsbo(
        m_frustums.size() * sizeof(SimpleFrustum),   // Size of SSBO
        NULL,                                        // SSBO data
        GL_MAP_READ_BIT,                             // Buffer flags
        m_gridFrustumBindingPoint);                  // Binding point

    return result;
}

void ForwardPlusRender::calGridFrustums()
{
    if (m_renderFlags.bits.skipCalGridFrustums == 0)
    {
        GLuint gridFrustumProgram = m_gridFrustumComputeShader.useProgram();

        // Set UBOs
        GLuint frustumSizeLocation = glGetUniformLocation(gridFrustumProgram, "frustumSize");
        glUniform1i(frustumSizeLocation, m_frustumNum[0] * m_frustumNum[1]);

        GLuint globalWidthLocation = glGetUniformLocation(gridFrustumProgram, "globalWidth");
        glUniform1i(globalWidthLocation, m_frustumNum[0]);

        GLuint groupWidthLocation = glGetUniformLocation(gridFrustumProgram, "groupSize");
        glUniform1i(groupWidthLocation, m_frustumSize[0]);

        UniformBufferMgr* pUboMgr = m_pScene->GetUniformBufferMgr();

        globalSizeUboIndex = pUboMgr->createUniformBuffer(GL_STATIC_DRAW, sizeof(m_frustumNum), (&(m_frustumNum[0])));
        pUboMgr->bindUniformBuffer(globalSizeUboIndex, gridFrustumProgram, "globalSizeUniformBlock");

        UINT resolutionUboIndex = pUboMgr->createUniformBuffer(GL_STATIC_DRAW, sizeof(m_resolution), &m_resolution);
        pUboMgr->bindUniformBuffer(resolutionUboIndex, gridFrustumProgram, "forwardPlusResolutionUniformBlock");

        // Calculate the frustums
        GLuint invProjMatLocation = glGetUniformLocation(gridFrustumProgram, "projMatInv");

        Math::Mat4 projMatrix = m_pScene->GetProjectionMatrix();
        glm::mat4 invProjMatrix = glm::inverse(ToGLMMat4(projMatrix));
        glUniformMatrix4fv(invProjMatLocation, 1, GL_FALSE, glm::value_ptr(invProjMatrix));

        m_gridFrustumComputeShader.compute();

        Shader::FinishProgram();

        m_renderFlags.bits.skipCalGridFrustums = 1;
    }
}

BOOL ForwardPlusRender::initTileLightList()
{
    BOOL result = TRUE;

    if (1 == m_renderFlags.bits.lightListUpdate)
    {
        SsboMgr* pSsboMgr = m_pScene->GetSsboMgr();

        m_tiledLightList.m_pLightListData = m_pScene->GetLightMgr()->GetLightData();

        UINT frunstumTotalNum = m_frustumNum[0] * m_frustumNum[1];

        pSsboMgr->addDynamicSsbo(
            frunstumTotalNum,
            NULL,
            GL_DYNAMIC_DRAW,
            m_tiledLightList.m_lightGridBindingPoint);

        pSsboMgr->addDynamicSsbo(
            frunstumTotalNum * m_pScene->GetLightMgr()->GetLightCount(),
            NULL,
            GL_DYNAMIC_DRAW,
            m_tiledLightList.m_lightIndexListBindingPoint);

        m_lightCullingComputeShader.setShader("LightCulling.cs");
        result = m_lightCullingComputeShader.compileShaderProgram();

        assert(result == TRUE);

        m_lightCullingComputeShader.setTotalThreadSize(
            m_frustumNum[0], // Number of frustum tiles on X
            m_frustumNum[1], // Number of frustum tiles on Y
            1);              // Keep Z to 1

        m_renderFlags.bits.lightListUpdate = 0;
    }

    return result;
}

void ForwardPlusRender::lightCulling()
{
    GLuint lightCullingProgram = m_lightCullingComputeShader.useProgram();

    UniformBufferMgr* pUboMgr = m_pScene->GetUniformBufferMgr();

    pUboMgr->bindUniformBuffer(globalSizeUboIndex, lightCullingProgram, "globalSizeUniformBlock");

    m_lightCullingComputeShader.compute();

    m_lightCullingComputeShader.FinishProgram();
}
