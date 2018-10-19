#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

Scene::Scene(Setting* pSetting)
    : m_pSetting(pSetting),
      m_backgroundColor(0.0f, 0.0f, 0.6f, 1.0f),
      m_pLightMgr(NULL),
      m_activeCameraIndex(0),
      m_pActiveCamera(NULL),
      m_uniformBufferMgr(),
      m_pGBuffer(NULL),
      useGlobalMaterial(FALSE),
      m_pSsao(NULL),
      m_pSkybox(NULL),
      m_pLightProbe(NULL),
      enableRealtimeLightProbe(FALSE),
      enableDebugDraw(FALSE)
{
    m_globalPbrMaterial.albedo    = Math::Vector3(0.6f, 0.6f, 0.6f);
    m_globalPbrMaterial.roughness = 0.3f;
    m_globalPbrMaterial.metallic  = 0.5f;
    m_globalPbrMaterial.fresnel   = 1.0f;
}

Scene::~Scene()
{
    for (Model* pModel : m_pSceneModelList)
    {
        SafeDelete(pModel);
    }
    m_pSceneModelList.clear();

    for (Camera* pCamera : m_pCameraList)
    {
        switch (pCamera->GetCameraType())
        {
        case CameraType::PROSPECTIVE_CAM:
            SafeDelete(static_cast<ProspectiveCamera*>(pCamera));
            break;
        case CameraType::ORTHOGRAPHIC_CAM:
            SafeDelete(static_cast<OrthographicCamera*>(pCamera));
            break;
        default:
            printf("Invalid camera type!\n");
            SafeDelete(pCamera);

            assert(FALSE);
            break;
        }
    }
    m_pCameraList.clear();

    for (Texture* pTexture : m_pTextureList)
    {
        switch (pTexture->GetTextureType())
        {
        case TextureType::TEXTURE_2D:
            SafeDelete(static_cast<Texture2D*>(pTexture));
            break;
        case TextureType::TEXTURE_3D:
            SafeDelete(static_cast<Texture3D*>(pTexture));
            break;
        case TextureType::TEXTURE_CUBEMAP:
            SafeDelete(static_cast<Cubemap*>(pTexture));
            break;
        default:
            printf("Invalid texture type!\n");
            SafeDelete(pTexture);

            assert(FALSE);
            break;
        }
    }
    m_pTextureList.clear();

    SafeDelete(m_pLightMgr);
    SafeDelete(m_pGBuffer);
    SafeDelete(m_pSsao);
    SafeDelete(m_pSkybox);
    SafeDelete(m_pLightProbe);

    for (Sprite* pSprite : m_pDebugSpriteList)
    {
        SafeDelete(pSprite);
    }

    SafeDelete(m_pForwardPlusRenderer);
}

BOOL Scene::initialize()
{
    BOOL status = TRUE;

    m_pLightMgr = new LightMgr(this);

    // Create light ubo
    status = m_pLightMgr->initialize();
    if (status == FALSE)
    {
        printf("Failed to initialize light manager!\n");
        assert(FALSE);
    }

    // Create resolution ubo
    m_resolutionUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW, sizeof(Resolution), &(m_pSetting->resolution));

    // Forward Phong rendering initialization
    status = initializePhongRendering();
    if (status == FALSE)
    {
        printf("Failed to initialize main scene Phong Rendering!\n");
        assert(FALSE);
    }

    // Forward PBR rendering initialization
    status = initializePBRendering();
    if (status == FALSE)
    {
        printf("Failed to initialize main scene PBRendering!\n");
        assert(FALSE);
    }

    // Deferred shading initialization
    if (m_pSetting->m_graphicsSetting.renderingMethod != FORWARD_RENDERING)
    {
        status = initializeDeferredRendering();
        if (status == FALSE)
        {
            printf("Failed to initialize G-Buffer!\n");
            assert(FALSE);
        }

        // SSAO only support deferred shading now
        if (useSSAO() == TRUE)
        {
            m_pSsao = new SSAO(this, m_pSetting);
            m_pSsao->initialize();
        }
    }

    if (status == FALSE)
    {
        Shader::AssertErrors();
    }

    // Light Probe
    m_pLightProbe = new LightProbe(this, Math::Vector3(3.0f, 0.0f, 0.0f), 0.1f, 1000.0f);
    m_pLightProbe->initialize();

    // Text rendering
    m_text.initialize();
    if (enableDebugDraw == TRUE)
    {
        initializeDebug();
    }

    // Forward+
    m_pForwardPlusRenderer = new ForwardPlusRender(this);
    m_pForwardPlusRenderer->initialize();

    return status;
}

void Scene::update(float deltaTime)
{
    UINT cameraCount = m_pCameraList.size();

    assert(cameraCount > 0);

#if _DEBUG
    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_1])
    {
        m_activeCameraIndex = 0;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_2])
    {
        m_activeCameraIndex = ((1 < cameraCount) ? 1 : m_activeCameraIndex);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_3])
    {
        m_activeCameraIndex = ((2 < cameraCount) ? 2 : m_activeCameraIndex);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_4])
    {
        m_activeCameraIndex = ((3 < cameraCount) ? 3 : m_activeCameraIndex);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_5])
    {
        m_activeCameraIndex = ((4 < cameraCount) ? 4 : m_activeCameraIndex);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_6])
    {
        m_activeCameraIndex = ((5 < cameraCount) ? 5 : m_activeCameraIndex);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_7])
    {
        m_activeCameraIndex = ((6 < cameraCount) ? 6 : m_activeCameraIndex);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_8])
    {
        m_activeCameraIndex = ((7 < cameraCount) ? 7 : m_activeCameraIndex);
    }
#endif

    m_pActiveCamera = m_pCameraList[m_activeCameraIndex];
    m_pActiveCamera->update(deltaTime);
    
    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_T])
    {
        m_pSetting->m_graphicsSetting.renderingMethod = FORWARD_RENDERING;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_Y])
    {
        m_pSetting->m_graphicsSetting.renderingMethod = DEFERRED_RENDERING;
        if (m_pGBuffer == NULL)
        {
            BOOL result = initializeDeferredRendering();
            if (result == FALSE)
            {
                printf("Failed to initialize G-Buffer!\n");
                assert(FALSE);
            }
        }
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_U])
    {
        m_pSetting->m_graphicsSetting.renderingMethod = FORWARD_PLUS_RENDERING;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_I])
    {
        m_pSetting->m_graphicsSetting.renderingMethod = DEFERRED_TILED_RENDERING;
    }

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_R])
    {
        m_pLightMgr->rotateLight(0, Math::Vector3(0.0f, 1.0f, 0.0f), glm::radians(5.0f));
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_L])
    {
        m_pLightMgr->rotateLight(0, Math::Vector3(0.0f, 1.0f, 0.0f), glm::radians(-5.0f));
    }

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_Z])
    {
        m_pSetting->m_graphicsSetting.shadowCasting = TRUE;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_X])
    {
        m_pSetting->m_graphicsSetting.shadowCasting = FALSE;
    }

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_G])
    {
        m_globalPbrMaterial.roughness += 0.01f;

        if (m_globalPbrMaterial.roughness > 1.0f)
        {
            m_globalPbrMaterial.roughness = 1.0f;
        }
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_H])
    {
        m_globalPbrMaterial.roughness -= 0.01f;

        if (m_globalPbrMaterial.roughness < 0.01f)
        {
            m_globalPbrMaterial.roughness = 0.01f;
        }
    }

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_J])
    {
        m_globalPbrMaterial.metallic += 0.01f;

        if (m_globalPbrMaterial.metallic > 1.0f)
        {
            m_globalPbrMaterial.metallic = 1.0f;
        }
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_K])
    {
        m_globalPbrMaterial.metallic -= 0.01f;

        if (m_globalPbrMaterial.metallic < 0.01f)
        {
            m_globalPbrMaterial.metallic = 0.01f;
        }
    }

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_C])
    {
        EnableSSAO();
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_V])
    {
        DisableSSAO();
    }

    if (enableDebugDraw == TRUE)
    {
        m_renderText = std::to_string(deltaTime);
    }

    if (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_PLUS_RENDERING ||
        m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_TILED_RENDERING)
    {
        m_pForwardPlusRenderer->update();
    }

    // Demo
    /*for (UINT i = 1; i < m_pLightMgr->GetLightCount(); ++i)
    {
        Light* pLight = m_pLightMgr->GetLight(i);

        if (pLight->GetLightType() == POINT_LIGHT)
        {
            PointLight* pPointLight = static_cast<PointLight*>(pLight);

            if (pPointLight->GetPos().y <= 100)
            {
                m_pLightMgr->translateLight(i, Math::Vector3(0.0f, 2.0f, 0.0f));
            }
            else
            {
                m_pLightMgr->translateLight(i, Math::Vector3(0.0f, -150.0f, 0.0f));
            }
        }
    }*/
}

void Scene::preDraw()
{
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Updating UBO data
    /*m_pLightMgr->updateLightUbo(m_sceneShader.GetShaderProgram(), "lightArrayUniformBlock");
    m_pLightMgr->updateLightUbo(m_pbrShader.GetShaderProgram(), "lightArrayUniformBlock");
    m_pLightMgr->updateLightUbo(m_deferredRenderingShader.GetShaderProgram(), "lightArrayUniformBlock");*/

    m_pLightMgr->updateLightSsbo();

    if (useGlobalMaterial == TRUE)
    {
        m_uniformBufferMgr.updateUniformBufferData(
            m_pbrMaterialUniformBufferIndex,
            m_globalPbrMaterial.GetOpaqueCookTorranceMaterialDataSize(),
            m_globalPbrMaterial.GetCookTorranceMaterialData());
    }
    /// End updating UBO data

    if (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_PLUS_RENDERING ||
        m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_TILED_RENDERING)
    {
        // Invoking frustum grid calculation and light culling
        m_pForwardPlusRenderer->draw();
    }

    /// Casting shadow
    if (m_pSetting->m_graphicsSetting.shadowCasting == TRUE)
    {
        shadowPass();
    }

    /// Casting light radiance on light probes
    if ((m_pLightProbe != NULL)                 &&
        ((enableRealtimeLightProbe     == TRUE) ||
         (m_pLightProbe->IsFirstDraw() == TRUE)))
    {
        m_pLightProbe->draw();
    }
    glViewport(0, 0, m_pSetting->resolution.width, m_pSetting->resolution.height);
}

void Scene::draw()
{
    if (m_skyboxImages.size() == CUBE_MAP_FACE_NUM)
    {
        m_pSkybox->draw();
    }

    if (m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_RENDERING ||
        m_pSetting->m_graphicsSetting.renderingMethod == FORWARD_PLUS_RENDERING)
    {
        /*if (m_pSsao == NULL)
        {
            m_pSsao = new SSAO(this, m_pSetting);
            m_pSsao->initialize();
        }

        assert(m_pSsao != NULL);
        m_pSsao->draw();*/
        drawScene();
    }
    else
    {
        m_pGBuffer->drawGBuffer();

        if (useSSAO() == TRUE)
        {
            if (m_pSsao == NULL)
            {
                m_pSsao = new SSAO(this, m_pSetting);
                m_pSsao->initialize();
            }

            assert(m_pSsao != NULL);
            m_pSsao->draw();
        }

        deferredDrawScene();
    }
}

void Scene::postDraw()
{
    float fps = 1.0f / stof(m_renderText);
    m_renderText = "SECONDS PER FRAME: " + m_renderText + "S\n";
    // m_renderText += "FPS: " + std::to_string(fps) + "\n";

    glm::vec3 camPos = m_pActiveCamera->GetTrans().GetPos();
    m_renderText += "CAMERA POSITION: (";
    m_renderText += std::to_string(camPos.x);
    m_renderText += ", ";
    m_renderText += std::to_string(camPos.y);
    m_renderText += ", ";
    m_renderText += std::to_string(camPos.z);
    m_renderText += ")\n";

    m_text.RenderText(this, m_renderText, Math::Vector2(50.0f, 700.0f));

    // TODO: Post processing

    debugDraw();
}

void Scene::drawScene()
{
    assert(m_pSetting->m_graphicsSetting.renderingMethod != DEFERRED_RENDERING);

    GLuint sceneShaderProgram;
    GLuint materialIndex;

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        Model* pModel = m_pSceneModelList[i];

        if (useGlobalMaterial == FALSE)
        {
            MaterialType modelMaterialType = pModel->GetModelMaterialType();

            switch (modelMaterialType)
            {
            case MaterialType::PHONG:
                sceneShaderProgram = m_sceneShader.useProgram();
                materialIndex = m_materialUniformBufferIndex;
                break;
            case MaterialType::COOKTORRANCE:
                sceneShaderProgram = m_pbrShader.useProgram();
                materialIndex = m_pbrMaterialUniformBufferIndex;
                break;
            default:
                printf("Unsupport material!\n");
                assert(FALSE);
                break;
            }

            pModel->updateMaterial(&m_uniformBufferMgr, materialIndex);
        }
        else
        {
            sceneShaderProgram = m_pbrShader.useProgram();
            materialIndex = m_pbrMaterialUniformBufferIndex;
            pModel->updateMaterial(&m_uniformBufferMgr, materialIndex);
        }

        GLint renderMethodHandle = glGetUniformLocation(sceneShaderProgram, "useForwardPlus");

        assert(renderMethodHandle >= 0);

        switch(m_pSetting->m_graphicsSetting.renderingMethod)
        {
            case FORWARD_RENDERING:
            {
                glUniform1ui(renderMethodHandle, 0);

                // TODO: Remove this UBOs from forward rendering
                m_uniformBufferMgr.bindUniformBuffer(
                    m_pForwardPlusRenderer->tileSizeUboIndex, sceneShaderProgram, "TileSizeUniformBlock");
                m_uniformBufferMgr.bindUniformBuffer(
                    m_pForwardPlusRenderer->globalSizeUboIndex, sceneShaderProgram, "GlobalSizeUniformBlock");

                break;
            }
            case FORWARD_PLUS_RENDERING:
            {
                glUniform1ui(renderMethodHandle, 1);
                /*m_uniformBufferMgr.bindUniformBuffer(
                    m_pForwardPlusRenderer->tileSizeUboIndex, sceneShaderProgram, "TileSizeUniformBlock");*/
                m_uniformBufferMgr.bindUniformBuffer(
                    m_pForwardPlusRenderer->globalSizeUboIndex, sceneShaderProgram, "GlobalSizeUniformBlock");
                break;
            }
            default:
                printf("Unsupport render method!\n");
                assert(FALSE);
                break;
        }
        
        glm::mat4 transMatrix[4] =
        {
            pModel->GetTrans()->GetTransMatrix(),
            ToGLMMat4(m_pActiveCamera->GetViewMatrix()),
            ToGLMMat4(m_pActiveCamera->GetProjectionMatrix()),
            glm::mat4()
        };
        transMatrix[3] = transMatrix[2] * transMatrix[1] * transMatrix[0];

        m_uniformBufferMgr.updateUniformBufferData(
            m_transUniformbufferIndex, sizeof(transMatrix), &(transMatrix[0]));

        GLint eyeHandle = glGetUniformLocation(sceneShaderProgram, "eyePos");
        glUniform3fv(eyeHandle, 1, glm::value_ptr(m_pActiveCamera->GetTrans().GetPos()));

        m_pLightMgr->GetShadowMgr()->
                readShadowMap(GL_TEXTURE4, sceneShaderProgram, "shadowMapSampler", 4);

        if (m_pLightProbe != NULL)
        {
            m_pLightProbe->readLightProbe(sceneShaderProgram, "lightProbeCubemap", GL_TEXTURE5);
        }

        pModel->draw(sceneShaderProgram);
    }

    Shader::FinishProgram();
}

void Scene::deferredDrawScene()
{
    assert(m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_RENDERING       ||
           m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_TILED_RENDERING);

    GLuint gShaderProgram = m_deferredRenderingShader.useProgram();
    m_pGBuffer->readGBuffer(gShaderProgram);

    Camera* activeCamPtr = m_pCameraList[m_activeCameraIndex];

    GLint eyeLocation        = glGetUniformLocation(gShaderProgram, "eyePos");
    GLint viewMatLocation    = glGetUniformLocation(gShaderProgram, "viewMat");
    GLint renderMethodHandle = glGetUniformLocation(gShaderProgram, "useForwardPlus");

    if (eyeLocation        >= 0 &&
        viewMatLocation    >= 0 &&
        renderMethodHandle >= 0)
    {
        glUniform3fv(eyeLocation, 1, glm::value_ptr(activeCamPtr->GetTrans().GetPos()));
        glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, glm::value_ptr(ToGLMMat4(activeCamPtr->GetViewMatrix())));

        if (m_pLightProbe != NULL)
        {
            m_pLightProbe->readLightProbe(gShaderProgram, "lightProbeCubemap", GL_TEXTURE6);
        }

        // Test
        GLint useSsaoLocation = glGetUniformLocation(gShaderProgram, "useSsao");
        if (useSSAO() == TRUE)
        {
            m_pSsao->bindSsaoTexture(GL_TEXTURE7, gShaderProgram, "ssaoTex", 7);

            glUniform1i(useSsaoLocation, 1);
        }
        else
        {
            glUniform1i(useSsaoLocation, 0);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        switch (m_pSetting->m_graphicsSetting.renderingMethod)
        {
            case DEFERRED_RENDERING:
            {
                glUniform1ui(renderMethodHandle, 0);
                break;
            }
            case DEFERRED_TILED_RENDERING:
            {
                glUniform1ui(renderMethodHandle, 1);
                m_uniformBufferMgr.bindUniformBuffer(
                    m_pForwardPlusRenderer->tileSizeUboIndex, gShaderProgram, "TileSizeUniformBlock");
                m_uniformBufferMgr.bindUniformBuffer(
                    m_pForwardPlusRenderer->globalSizeUboIndex, gShaderProgram, "GlobalSizeUniformBlock");
                break;
            }
            default:
                printf("Unsupport render method!\n");
                assert(FALSE);
                break;
        }

        // Casting shadow map
        m_pLightMgr->GetShadowMgr()->
                readShadowMap(GL_TEXTURE8, gShaderProgram, "shadowMapSampler", 8);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_pGBuffer->draw();
        glDisable(GL_BLEND);
    }
    else
    {
        printf("Unable to get wvp matrix location in shader\n");
        assert(FALSE);
    }

    Shader::FinishProgram();
}

void Scene::setSceneShader(
    char* const vertexShaderFile,
    char* const fragmentShaderFile)
{
    m_sceneShader.setShaderFiles(vertexShaderFile, fragmentShaderFile);
}

void Scene::shadowPass()
{
    ShadowMap* pShadowMap = m_pLightMgr->GetLight(0)->GetShadowMap();

    m_pLightMgr->updateLightShadow();
    m_pLightMgr->castShadow();
}

BOOL Scene::initializePhongRendering()
{
    BOOL status = TRUE;

    //Compile shaders
    m_sceneShader.setShaderFiles("MainScene.vert", "MainScene.frag");
    status = m_sceneShader.compileShaderProgram();

    if (status == FALSE)
    {
        Shader::AssertErrors("Failed to compile main scene shader.");
    }

    /// UBOs initialization
    m_transUniformbufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW, sizeof(Math::Mat4) * 4, NULL);

    m_uniformBufferMgr.bindUniformBuffer(
        m_transUniformbufferIndex,
        m_sceneShader.GetShaderProgram(),
        "transUniformBlock");

    /*m_pLightMgr->bindLightUbo(
        &m_uniformBufferMgr,
        m_sceneShader.GetShaderProgram(),
        "lightArrayUniformBlock");*/

    // Material ubo
    m_materialUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(
            GL_DYNAMIC_DRAW, sizeof(SpecularMaterial::m_specularMaterialData), NULL);

    m_uniformBufferMgr.bindUniformBuffer(
        m_materialUniformBufferIndex, m_sceneShader.GetShaderProgram(), "material");

    // Shadow map resolution ubo
    /*m_uniformBufferMgr.bindUniformBuffer(
        m_pShadowMap->GetShadowResolutionUniformBufferIndex(),
        m_sceneShader.GetShaderProgram(),
        "shadowMapResolutionUniformBlock");*/
    /// UBOs initialization end

    return status;
}

BOOL Scene::initializePBRendering()
{
    BOOL status = TRUE;

    // PBR shader
    m_pbrShader.setShaderFiles("MainScene.vert", "MainSceneCookTorrance.frag");
    status = m_pbrShader.compileShaderProgram();

    if (status == FALSE)
    {
        Shader::AssertErrors("Failed to compile PBR shader.");
    }

    /// UBOs initialization
    m_uniformBufferMgr.bindUniformBuffer(
        m_transUniformbufferIndex,
        m_pbrShader.GetShaderProgram(),
        "transUniformBlock");

    m_uniformBufferMgr.bindUniformBuffer(
        m_resolutionUniformBufferIndex,
        m_pbrShader.GetShaderProgram(),
        "RenderingResolutionBlock");

    /*m_pLightMgr->bindLightUbo(
        &m_uniformBufferMgr,
        m_pbrShader.GetShaderProgram(),
        "lightArrayUniformBlock");*/

    m_pbrMaterialUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(
            GL_DYNAMIC_DRAW, CookTorranceMaterial::GetOpaqueCookTorranceMaterialDataSize(), NULL);

    m_uniformBufferMgr.bindUniformBuffer(
        m_pbrMaterialUniformBufferIndex, m_pbrShader.GetShaderProgram(), "CookTorranceMaterialUniformBlock");

    // Shadow map resolution ubo
    /*m_uniformBufferMgr.bindUniformBuffer(
        m_pShadowMap->GetShadowResolutionUniformBufferIndex(),
        m_pbrShader.GetShaderProgram(),
        "shadowMapResolutionUniformBlock");*/
    /// UBOs initialization end

    return status;
}

void Scene::initializeDebug()
{
    for(int i = 0; i < 5; ++i)
    {        
        m_pDebugSpriteList.push_back(
            new Sprite(this, Math::Vector3(50.0f, 500.0f - i * 100.0f, 0.0f), { 100.0f, 100.0f }));
        m_pDebugSpriteList[i]->initialize();
    }
}

void Scene::debugDraw()
{
    if (enableDebugDraw == TRUE)
    {
        m_renderText = "SECONDS PER FRAME: " + m_renderText + "MS\n";

        switch (m_pSetting->m_graphicsSetting.renderingMethod)
        {
            case FORWARD_RENDERING:
                m_renderText += "FORWARD_RENDERING\n";
                break;
            case DEFERRED_RENDERING:
                m_renderText += "DEFERRED_RENDERING\n";
                break;
            case FORWARD_PLUS_RENDERING:
                m_renderText += "FORWARD_PLUS_RENDERING\n";
                break;
            case DEFERRED_TILED_RENDERING:
                m_renderText += "DEFERRED_TILED_RENDERING\n";
                break;
            default:
                printf("Unsupport render method!\n");
                assert(FALSE);
                break;
        }

        glm::vec3 camPos = m_pActiveCamera->GetTrans().GetPos();
        m_renderText += "CAMERA POSITION: (";
        m_renderText += std::to_string(camPos.x);
        m_renderText += ", ";
        m_renderText += std::to_string(camPos.y);
        m_renderText += ", ";
        m_renderText += std::to_string(camPos.z);
        m_renderText += ")\n";

        m_text.RenderText(this, m_renderText, Math::Vector2(50.0f, 700.0f));

        if (m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_RENDERING ||
            m_pSetting->m_graphicsSetting.renderingMethod == DEFERRED_TILED_RENDERING)
        {
            if (m_pDebugSpriteList.size() == 0)
            {
                initializeDebug();
            }

            for (int i = 0; i < 5; ++i)
            {
                m_pDebugSpriteList[i]->BindTexture(static_cast<Texture2D*>(m_pGBuffer->GetGBufferTexture(i)));
                m_pDebugSpriteList[i]->draw();
            }
        }
    }
}

BOOL Scene::initializeDeferredRendering()
{
    BOOL status = TRUE;

    m_pGBuffer = new GBuffer(this, m_pSetting->resolution.width, m_pSetting->resolution.height);
    status = m_pGBuffer->initialize();
    
    if (useGlobalMaterial == TRUE)
    {
        m_pGBuffer->UseGlobalMaterial();
    }
    else
    {
        m_pGBuffer->UseLocalMaterial();
    }

    if (status == FALSE)
    {
        Shader::AssertErrors();
    }

    // G-Buffer shaders
    m_deferredRenderingShader.setShaderFiles("MainSceneDefferedDraw.vert", "MainSceneDefferedDraw.frag");
    status = m_deferredRenderingShader.compileShaderProgram();

    if (status == FALSE)
    {
        Shader::AssertErrors("Failed to compile deffered draw shader.");
    }

    /// UBOs initialization

    // Bind Light Ubo
    /*m_pLightMgr->bindLightUbo(
        &m_uniformBufferMgr,
        m_deferredRenderingShader.GetShaderProgram(),
        "lightArrayUniformBlock");*/

    // Resolution ubo
    m_uniformBufferMgr.bindUniformBuffer(
        m_resolutionUniformBufferIndex,
        m_deferredRenderingShader.GetShaderProgram(),
        "RenderingResolutionBlock");

    // Shadow map resolution ubo
    /*m_uniformBufferMgr.bindUniformBuffer(
        m_pShadowMap->GetShadowResolutionUniformBufferIndex(),
        m_deferredRenderingShader.GetShaderProgram(),
        "shadowMapResolutionUniformBlock");*/
    /// UBOs initialization end

    return status;
}

void Scene::addModel(
    const std::string& modelFile,
    const std::string& materialFile,
    Trans*             modelTrans)
{
    Model* pModel = new Model(modelFile, materialFile, modelTrans);
    m_pSceneModelList.push_back(pModel);
}

void Scene::addProspectiveCamera(
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    float speed,
    float aspectRatio,
    float nearClip,
    float farClip,
    float fov)
{
    m_pCameraList.push_back(
        new ProspectiveCamera(pos, center, up, speed, aspectRatio, nearClip, farClip, fov));
}

void Scene::addOrthographicCamera(
    const glm::vec3&    pos,
    const glm::vec3&    center,
    const glm::vec3&    up,
    const float         speed,
    const BxsRectangle  viewport,
    const float         nearClip,
    const float         farClip) 
{
    m_pCameraList.push_back(
        new OrthographicCamera(pos, center, up, speed, viewport, nearClip, farClip));
}

void Scene::addTexture(
    const std::string& textureFile,
    const GLenum       textureType,
    const GLenum       format,
    const GLenum       type,
    const GLenum       wrapMethod,
    const BOOL         mipmap)
{
    switch (textureType)
    {
    case GL_TEXTURE_2D:
        m_pTextureList.push_back(
            new Texture2D(textureFile, 1 /*TODO: Multi-sampled texture*/, format, type, wrapMethod, mipmap));
        break;
    case GL_TEXTURE_3D:
        // TODO
        break;
    case GL_TEXTURE_CUBE_MAP:
        // TODO
        break;
    default:
        printf("Unsupport texture type!\n");
        assert(FALSE);
        break;
    }
}

void Scene::addSkyboxImage(
    const char* const x_frontImg,
    const char* const x_backImg,
    const char* const y_frontImg,
    const char* const y_backImg,
    const char* const z_frontImg,
    const char* const z_backImg)
{
    m_skyboxImages = 
    {
        x_frontImg,
        x_backImg,
        y_frontImg,
        y_backImg,
        z_frontImg,
        z_backImg
    };

    // Skybox
    if (m_skyboxImages.size() == CUBE_MAP_FACE_NUM)
    {
        if (m_pSkybox == NULL)
        {
            m_pSkybox = new Skybox(this, m_skyboxImages);
        }
        else
        {
            m_pSkybox->GetSkyboxCubemap().update(m_skyboxImages);
        }

        m_pSkybox->initialize();
    }
}

void Scene::setGlobalMaterial(
    Material* pMaterial)
{
    for (Model* pModel : m_pSceneModelList)
    {
        pModel->updateMaterialData(pMaterial);
    }
}

void Scene::enableSceneLocalMaterial()
{
    useGlobalMaterial = FALSE;
    if (m_pGBuffer != NULL) { m_pGBuffer->UseLocalMaterial(); }

    for (Model* pModel : m_pSceneModelList)
    {
        pModel->UseLocalMaterial();
    }
}

void Scene::disableSceneLocalMaterial()
{
    useGlobalMaterial = TRUE;
    if (m_pGBuffer != NULL) { m_pGBuffer->UseGlobalMaterial(); }

    for (Model* pModel : m_pSceneModelList)
    {
        pModel->UseGlobalMaterial();
    }
}

BOOL Scene::useSSAO()
{
    BOOL result = TRUE;
    AmbientOcclutionSetting ambientOcclutionSetting =
        m_pSetting->m_graphicsSetting.ambientOcclutionSetting;

    result = ((ambientOcclutionSetting.ambientOcclusion !=
        AmbientOcclusion::AMBIENTOCCLUSION_NONE) ? TRUE : FALSE);

    return result;
}