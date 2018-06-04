#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

//Image loader
#include "stb_image.h"

Scene::Scene(const Setting& setting)
    : m_backgroundColor(0.0f, 0.0f, 0.6f, 1.0f),
      m_directionalLight(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
      m_pointLight(Vector3(0.0f, 5.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 10.0f),
      m_activeCamera(0),
      m_uniformBufferMgr(128),
      m_pShadowMap(NULL),
      m_pGBuffer(NULL)
{
    this->setting = setting;
}

BOOL Scene::initialize()
{
    BOOL status = TRUE;

    //Compile shaders
    m_sceneShader.setShaderFiles("MainSceneShadowMap.vert", "MainSceneShadowMap.frag");
    status = m_sceneShader.linkProgram();

    if (status == FALSE)
    {
        m_sceneShader.assertErrors();
    }

    /// UBOs initialization
    m_transUniformbufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW, sizeof(Mat4) * 4, nullptr);

    m_uniformBufferMgr.bindUniformBuffer(
        m_transUniformbufferIndex,
        m_sceneShader.GetShaderProgram(),
        "trans");

    // Directional light ubo
    m_directionalLightUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW,
                                               m_directionalLight.getDataSize(),
                                               m_directionalLight.getDataPtr());

    m_uniformBufferMgr.bindUniformBuffer(
        m_directionalLightUniformBufferIndex,
        m_sceneShader.GetShaderProgram(),
        "directionalLightUniformBlock");

    // Point light ubo
    m_pointLightUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW,
            m_pointLight.getDataSize(),
            m_pointLight.getDataPtr());

    m_uniformBufferMgr.bindUniformBuffer(
        m_pointLightUniformBufferIndex,
        m_sceneShader.GetShaderProgram(),
        "pointLightUniformBlock");

    // Material ubo
    m_materialUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(
            GL_DYNAMIC_DRAW, sizeof(SpecularMaterial::m_materialData), NULL);
    m_uniformBufferMgr.bindUniformBuffer(
        m_materialUniformBufferIndex, m_sceneShader.GetShaderProgram(), "material");
    ///

    // Shadow map test
    status = initializeShadowMap();
    if (status == FALSE)
    {
        printf("Failed to initialize shadow map!\n");
        assert(FALSE);
    }

    // Deferred shading test
    if (setting.m_graphicsSetting.renderingMethod != FORWARD_RENDERING)
    {
        status = initializeDeferredRendering();
        if (status == FALSE)
        {
            printf("Failed to initialize G-Buffer!\n");
            assert(FALSE);
        }
    }

    if (status == FALSE)
    {
        m_sceneShader.assertErrors();
    }

    return status;
}

void Scene::update(float deltaTime)
{
    UINT cameraCount = m_pCameraList.size();

    assert(cameraCount > 0);

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_1])
    {
        m_activeCamera = 0;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_2])
    {
        m_activeCamera = ((1 < cameraCount) ? 1 : m_activeCamera);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_3])
    {
        m_activeCamera = ((2 < cameraCount) ? 2 : m_activeCamera);
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_4])
    {
        m_activeCamera = ((3 < cameraCount) ? 3 : m_activeCamera);
    }

    m_pCameraList[m_activeCamera]->update(deltaTime);
    
    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_N])
    {
        setting.m_graphicsSetting.renderingMethod = FORWARD_RENDERING;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_M])
    {
        setting.m_graphicsSetting.renderingMethod = DEFERRED_RENDERING;
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

    //m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(5.0f));
}

void Scene::draw()
{
    GLfloat timeValue = static_cast<GLfloat>(glfwGetTime());

    shadowPass();

    m_uniformBufferMgr.updateUniformBufferData(
            m_directionalLightUniformBufferIndex,
            m_directionalLight.getDataSize(),
            m_directionalLight.getDataPtr());

    m_uniformBufferMgr.updateUniformBufferData(
            m_pointLightUniformBufferIndex,
            m_pointLight.getDataSize(),
            m_pointLight.getDataPtr());

    if (setting.m_graphicsSetting.renderingMethod == RenderingMethod::FORWARD_RENDERING)
    {
        drawScene();
    }
    else
    {
        m_pGBuffer->drawGBuffer();
        deferredDrawScene();
    }
}

Scene::~Scene()
{
    for (Model* model : m_pSceneModelList)
    {
        SafeDelete(model);
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
            printf("Invalid camera type");
            assert(FALSE);

            SafeDelete(pCamera);
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
        case TextureType::TEXTURE_CUBEBOX:
            SafeDelete(static_cast<TextureCube*>(pTexture));
            break;
        default:
            printf("Invalid texture type!\n");
            SafeDelete(pTexture);

            assert(FALSE);
            break;
        }
    }
    m_pTextureList.clear();

    SafeDelete(m_pShadowMap);
    SafeDelete(m_pGBuffer);
}

void Scene::setSceneShader(
    char* const vertexShaderFile,
    char* const fragmentShaderFile)
{
    m_sceneShader.setShaderFiles(vertexShaderFile, fragmentShaderFile);
}

BOOL Scene::initializeShadowMap()
{
    BOOL result = TRUE;

    m_pShadowMap = new ShadowMap(
        static_cast<Light*>(&m_directionalLight), setting.width * 2, setting.height * 2,
        setting.m_graphicsSetting.antialasing);

    result = m_pShadowMap->initialize();

    return result;
}

void Scene::shadowPass()
{
    m_pShadowMap->update(&m_directionalLight);

    glCullFace(GL_FRONT);
    m_pShadowMap->drawShadowMap(this);
    glCullFace(GL_BACK);
}

void Scene::drawScene()
{
    m_sceneShader.useProgram();

    //ProspectiveCamera* activeCamPtr = static_cast<ProspectiveCamera*>(m_pCameraList[m_activeCamera]);
    Camera* activeCamPtr = m_pCameraList[m_activeCamera];

#if 0
    Vector3 camPos = activeCamPtr->GetTrans().GetPos();
    printf("%f %f %f\n", camPos.x, camPos.y, camPos.z);
#endif

    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        Model* pModel = m_pSceneModelList[i];

        glm::mat4 transMatrix[4] =
        {
            pModel->m_pTrans->GetTransMatrix(),
            activeCamPtr->GetViewMatrix(),
            activeCamPtr->GetProjectionMatrix(),
            glm::mat4()
        };
        transMatrix[3] = transMatrix[2] * transMatrix[1] * transMatrix[0];

        m_uniformBufferMgr.updateUniformBufferData(
            m_transUniformbufferIndex, sizeof(transMatrix), &(transMatrix[0]));

        GLint eyeHandle = glGetUniformLocation(m_sceneShader.GetShaderProgram(), "eyePos");
        glUniform3fv(eyeHandle, 1, glm::value_ptr(activeCamPtr->GetTrans().GetPos()));

        m_pTextureList[0]->bindTexture(GL_TEXTURE0, m_sceneShader.GetShaderProgram(), "sampler", 0);

        glm::mat4 lightTransWVP  = m_pShadowMap->GetLightTransVP() *
                                   pModel->m_pTrans->GetTransMatrix();

        GLint lightTransHandle = glGetUniformLocation(m_sceneShader.GetShaderProgram(), "lightTransWVP");
        glUniformMatrix4fv(lightTransHandle, 1, GL_FALSE, glm::value_ptr(lightTransWVP));

        m_pShadowMap->readShadowMap(GL_TEXTURE1, m_sceneShader.GetShaderProgram(), "shadowMapSampler", 1);

        pModel->updateMaterial(&m_uniformBufferMgr, m_materialUniformBufferIndex);
        pModel->draw();
    }

    m_sceneShader.finishProgram();
}

BOOL Scene::initializeDeferredRendering()
{
    BOOL status = TRUE;

    // G-Buffer shader
    m_defferedRendingShader.setShaderFiles("MainSceneDefferedDraw.vert", "MainSceneDefferedDraw.frag");
    m_defferedRendingShader.linkProgram();

    m_pGBuffer = new GBuffer(this, setting.width, setting.height);
    status = m_pGBuffer->initialize();

    if (status == FALSE)
    {
        m_sceneShader.assertErrors();
    }

    m_uniformBufferMgr.bindUniformBuffer(
        m_directionalLightUniformBufferIndex,
        m_defferedRendingShader.GetShaderProgram(),
        "directionalLightUniformBlock");

    return status;
}

void Scene::deferredDrawScene()
{
    assert(setting.m_graphicsSetting.renderingMethod == RenderingMethod::DEFERRED_RENDERING);

    GLuint gShaderProgram = m_defferedRendingShader.useProgram();
    m_pGBuffer->readGBuffer(gShaderProgram);

    Camera* activeCamPtr = m_pCameraList[m_activeCamera];
    
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        Model* pModel = m_pSceneModelList[i];

        glm::mat4 worldMatrix = pModel->m_pTrans->GetTransMatrix();
        glm::mat4 viewMatrix = activeCamPtr->GetViewMatrix();
        glm::mat4 prospectMatrix = activeCamPtr->GetProjectionMatrix();
        glm::mat4 wvp = prospectMatrix * viewMatrix * worldMatrix;

        GLint tranMatrixLocation = glGetUniformLocation(gShaderProgram, "wvp");
        GLint eyeLocation        = glGetUniformLocation(gShaderProgram, "eyePos");

        if (tranMatrixLocation >= 0 && eyeLocation >= 0)
        {
            glUniformMatrix4fv(tranMatrixLocation, 1, GL_FALSE, glm::value_ptr(wvp));
            glUniform3fv(eyeLocation, 1, glm::value_ptr(activeCamPtr->GetTrans().GetPos()));

            pModel->drawModelPos();
        }
        else
        {
            printf("Unable to get wvp matrix location in shadowMap shader");
            assert(FALSE);
        }
    }

    m_defferedRendingShader.finishProgram();
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
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    const float      speed,
    const Rectangle  viewport,
    const float      nearClip,
    const float      farClip) 
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
            new Texture2D(textureFile, format, type, wrapMethod, mipmap));
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
