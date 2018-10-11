#include "ForwardPlusRender.h"

#include "../Context/Scene.h"

#define DEFAULT_FRUSTUM_SIZE_X 16
#define DEFAULT_FRUSTUM_SIZE_Y 16

using Math::Vector4;
using Math::Mat4;

ForwardPlusRender::ForwardPlusRender(
    Scene* pScene)
    : m_pScene(pScene),
      m_pShadowMap(NULL)
{
    m_resolution = m_pScene->GetSetting()->resolution;

    m_frustumSize[0] = DEFAULT_FRUSTUM_SIZE_X;
    m_frustumSize[1] = DEFAULT_FRUSTUM_SIZE_Y;

    m_frustumNum[0] =
        static_cast<UINT>(std::ceil(static_cast<float>(m_resolution.width) / static_cast<float>(m_frustumSize[0])));
    m_frustumNum[1] =
        static_cast<UINT>(std::ceil(static_cast<float>(m_resolution.height) / static_cast<float>(m_frustumSize[1])));

    m_frustums.resize(m_frustumNum[0] * m_frustumNum[1] + 1);

    m_gridFrustumBindingPoint                     = 0;
    m_tiledLightList.m_lightGridBindingPoint      = 1;
    m_tiledLightList.m_lightIndexListBindingPoint = 2;

    m_renderFlags.value = 0;
}

ForwardPlusRender::~ForwardPlusRender()
{
    SafeDelete(m_pShadowMap);
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

    result = initTileLightList();

    assert(result == TRUE);

    LightMgr* pLightMgr = m_pScene->GetLightMgr();
    pLightMgr->bindLightUbo(
        m_pScene->GetUniformBufferMgr(), m_lightCullingComputeShader.GetShaderProgram(), "lightArrayUniformBlock");

    Setting* pSetting = m_pScene->GetSetting();
    m_camDepthBuffer.createFramebufferTexture2D(GL_TEXTURE0,
                                                GL_DEPTH_ATTACHMENT,
                                                GL_COLOR_ATTACHMENT0,
                                                pSetting->resolution.width,
                                                pSetting->resolution.height,
                                                1,
                                                GL_DEPTH_COMPONENT,
                                                GL_DEPTH_COMPONENT32F,
                                                GL_FLOAT,
                                                GL_CLAMP_TO_BORDER,
                                                FALSE);

    return result;
}

void ForwardPlusRender::update()
{
    updateLightData();
}

void ForwardPlusRender::draw()
{
    calGridFrustums();

    renderViewDepthBuffer();

    lightCulling();

    // Sample of mapping SSBO
    /*SsboMgr* pSsboMgr = m_pScene->GetSsboMgr();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pSsboMgr->GetSsbo(1)->GetSsboHandle());
    UINT* pData = static_cast<UINT*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

    for (int i = 0; i < 4; ++i)
    {
        gpuFrustumsPtr[i] = m_tests[i];
    }

    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);*/
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

    return result;
}

void ForwardPlusRender::calGridFrustums()
{
    if (m_renderFlags.bits.skipCalGridFrustums == FALSE)
    {
        // Generate frustum SSBO
        if (m_renderFlags.bits.skipGenerateFrustumSsbo == FALSE)
        {
            Camera* pCam = m_pScene->GetActivateCamera();

            m_frustums[0].sidePlanes[0].N = Math::Vector3(pCam->GetNearClip(), pCam->GetFarClip(), 0.0f);
            m_frustums[0].sidePlanes[0].d = 0.0f;

            m_gridFrustumSsboHandle = m_pScene->GetSsboMgr()->addStaticSsbo(
                m_frustums.size() * sizeof(Frustum),                   // Size of SSBO
                m_frustums.data(),                                     // SSBO data
                GL_MAP_READ_BIT,                                       // Buffer flags
                m_gridFrustumBindingPoint);                            // Binding point

            // Clear the CPU memory usage
            m_frustums.clear();

            m_renderFlags.bits.skipGenerateFrustumSsbo = TRUE;
        }

        m_gridFrustumComputeShader.setThreadGroupSize(m_frustumSize[0], m_frustumSize[1], 1);
        m_gridFrustumComputeShader.calGroupNum();

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
        pUboMgr->bindUniformBuffer(globalSizeUboIndex, gridFrustumProgram, "GlobalSizeUniformBlock");

        UINT resolutionUboIndex = pUboMgr->createUniformBuffer(GL_STATIC_DRAW, sizeof(m_resolution), &m_resolution);
        pUboMgr->bindUniformBuffer(resolutionUboIndex, gridFrustumProgram, "ForwardPlusResolutionUniformBlock");

        // Calculate the frustums
        GLuint invProjMatLocation = glGetUniformLocation(gridFrustumProgram, "projMatInv");

        Math::Mat4 projMatrix = m_pScene->GetProjectionMatrix();
        glm::mat4 invProjMatrix = glm::inverse(ToGLMMat4(projMatrix));
        glUniformMatrix4fv(invProjMatLocation, 1, GL_FALSE, glm::value_ptr(invProjMatrix));

        m_gridFrustumComputeShader.compute();

        Shader::FinishProgram();

        m_renderFlags.bits.skipCalGridFrustums = TRUE;
    }
}

BOOL ForwardPlusRender::initTileLightList()
{
    BOOL result = TRUE;

    m_lightCullingComputeShader.setShader("LightCulling.cs");
    result = m_lightCullingComputeShader.compileShaderProgram();

    assert(result == TRUE);

    // Initialize light index list counter
    UINT initCounterValue = 0;
    glGenBuffers(1, &m_tiledLightList.m_lightGridCounterHandle);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_tiledLightList.m_lightGridCounterHandle);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(UINT), &initCounterValue, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_tiledLightList.m_lightGridCounterHandle);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

    GLuint lightCullingProgram = m_lightCullingComputeShader.useProgram();

    GLuint frustumSizeLocation = glGetUniformLocation(lightCullingProgram, "frustumSize");
    glUniform1ui(frustumSizeLocation, m_frustumNum[0] * m_frustumNum[1]);

    UniformBufferMgr* pUboMgr = m_pScene->GetUniformBufferMgr();
    tileSizeUboIndex = pUboMgr->createUniformBuffer(GL_STATIC_DRAW, sizeof(m_frustumSize), (&(m_frustumSize[0])));
    pUboMgr->bindUniformBuffer(tileSizeUboIndex, lightCullingProgram, "TileSizeUniformBlock");

    Shader::FinishProgram();

    return result;
}

void ForwardPlusRender::updateLightData()
{
    if (TRUE == m_renderFlags.bits.lightListUpdate)
    {
        SsboMgr* pSsboMgr = m_pScene->GetSsboMgr();

        m_tiledLightList.m_pLightListData = m_pScene->GetLightMgr()->GetLightData();
        m_tiledLightList.m_lightNum = m_pScene->GetLightMgr()->GetLightCount();

        UINT frunstumTotalNum = m_frustumNum[0] * m_frustumNum[1];

        pSsboMgr->addDynamicSsbo(
            frunstumTotalNum * sizeof(TiledLightList::LightTile),
            NULL,
            GL_DYNAMIC_DRAW,
            m_tiledLightList.m_lightGridBindingPoint);

        pSsboMgr->addDynamicSsbo(
            frunstumTotalNum * m_tiledLightList.m_lightNum * sizeof(UINT),
            NULL,
            GL_DYNAMIC_DRAW,
            m_tiledLightList.m_lightIndexListBindingPoint);

        m_lightCullingComputeShader.setTotalThreadSize(
            m_frustumNum[0], // Number of frustum tiles on X
            m_frustumNum[1], // Number of frustum tiles on Y
            1);              // Keep Z to 1

        m_renderFlags.bits.lightListUpdate = 0;

        // Updating light data
        GLint lightNumLocation = glGetUniformLocation(m_lightCullingComputeShader.useProgram(), "lightNum");
        if (lightNumLocation >= 0)
        {
            glUniform1ui(lightNumLocation, m_pScene->GetLightMgr()->GetLightCount());
        }
        else
        {
            printf("Can't find light number uniform. \n");
        }

        m_pScene->GetLightMgr()->updateLightUbo(
            m_lightCullingComputeShader.GetShaderProgram(), "lightArrayUniformBlock");
    }

    if (FALSE == m_renderFlags.bits.skipShadowMapGenerating)
    {
        if (m_pShadowMap != NULL)
        {
            SafeDelete(m_pShadowMap);
        }

        if (m_pShadowMap == NULL)
        {
            m_pShadowMap = new ShadowMap(m_pScene, m_pScene->GetActivateCamera(), m_pScene->GetSetting()->resolution);
            m_pShadowMap->initialize();
        }

        m_renderFlags.bits.skipShadowMapGenerating = TRUE;
    }
}

void ForwardPlusRender::renderViewDepthBuffer()
{
    assert(m_pShadowMap != NULL);

    m_camDepthBuffer.setRenderTargets();

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    m_pShadowMap->drawShadowMap(m_pScene);

    m_camDepthBuffer.finishDrawFramebuffer();
}

void ForwardPlusRender::lightCulling()
{
    GLuint lightCullingProgram = m_lightCullingComputeShader.useProgram();

    GLuint viewMatLocation = glGetUniformLocation(m_lightCullingComputeShader.GetShaderProgram(), "viewMat");
    glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, glm::value_ptr(ToGLMMat4(m_pScene->GetActivateCamera()->GetViewMatrix())));

    UniformBufferMgr* pUboMgr = m_pScene->GetUniformBufferMgr();

    pUboMgr->bindUniformBuffer(globalSizeUboIndex, lightCullingProgram, "GlobalSizeUniformBlock");
    pUboMgr->bindUniformBuffer(tileSizeUboIndex, lightCullingProgram, "TileSizeUniformBlock");
    pUboMgr->bindUniformBuffer(
        m_pScene->m_resolutionUniformBufferIndex, lightCullingProgram, "RenderingResolutionBlock");

    m_camDepthBuffer.getTexturePtr(GL_TEXTURE0)
        ->bindTexture(GL_TEXTURE0, lightCullingProgram, "depthTexture");

    // Generate light index list and light grid
    m_lightCullingComputeShader.compute();

    // Finish light culling
    m_lightCullingComputeShader.FinishProgram();

    // Reset atomic buffer counter
    UINT initCounterValue = 0;
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_tiledLightList.m_lightGridCounterHandle);
    glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(UINT), &initCounterValue);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}
