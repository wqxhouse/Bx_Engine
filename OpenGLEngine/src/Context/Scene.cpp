#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

//Image loader
#include "stb_image.h"

Scene::Scene(const Setting& m_setting)
    : m_backgroundColor(0.0f, 0.0f, 0.6f, 1.0f),
      m_directionalLight(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
      //m_directionalLight(Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
      m_pointLight(Vector3(0.0f, 5.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 10.0f),
      m_activeCamera(0),
      m_uniformBufferMgr(128),
      m_pShadowMap(NULL),
      m_pGBuffer(NULL)
{
    this->m_setting = m_setting;
}

BOOL Scene::initialize()
{
    BOOL status = TRUE;

    status = initializePhongRendering();
    if (status == FALSE)
    {
        printf("Failed to initialize main scene Phong Rendering!\n");
        assert(FALSE);
    }

    // PBR rendering initialization
    status = initializePBRendering();
    if (status == FALSE)
    {
        printf("Failed to initialize main scene PBRendering!\n");
        assert(FALSE);
    }

    // Shadow map test
    status = initializeShadowMap();
    if (status == FALSE)
    {
        printf("Failed to initialize shadow map!\n");
        assert(FALSE);
    }

    // Shadow map resolution ubo
    /*m_uniformBufferMgr.bindUniformBuffer(
        m_pShadowMap->GetShadowResolutionUniformBufferIndex(),
        m_sceneShader.GetShaderProgram(),
        "shadowMapResolutionUniformBlock"
    );*/
    
    // Deferred shading test
    if (m_setting.m_graphicsSetting.renderingMethod != FORWARD_RENDERING)
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
        Shader::AssertErrors();
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
        m_setting.m_graphicsSetting.renderingMethod = FORWARD_RENDERING;
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_M])
    {
        m_setting.m_graphicsSetting.renderingMethod = DEFERRED_RENDERING;
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

    if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_R])
    {
        m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(5.0f));
    }
    else if (1 == callbackInfo.keyboardCallBack[GLFW_KEY_L])
    {
        m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(-5.0f));
    }
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

    glViewport(0, 0, m_setting.width, m_setting.height);

    if (m_setting.m_graphicsSetting.renderingMethod == RenderingMethod::FORWARD_RENDERING)
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
        this, static_cast<Light*>(&m_directionalLight), m_setting.width * 2, m_setting.height * 2,
        m_setting.m_graphicsSetting.antialasing);

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

BOOL Scene::initializePhongRendering()
{
    BOOL status = TRUE;

    //Compile shaders
    m_sceneShader.setShaderFiles("MainScene.vert", "MainScene.frag");
    status = m_sceneShader.linkProgram();

    if (status == FALSE)
    {
        Shader::AssertErrors();
    }

    /// UBOs initialization
    m_transUniformbufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW, sizeof(Mat4) * 4, nullptr);

    m_uniformBufferMgr.bindUniformBuffer(
        m_transUniformbufferIndex,
        m_sceneShader.GetShaderProgram(),
        "transUniformBlock");

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
            GL_DYNAMIC_DRAW, sizeof(SpecularMaterial::m_specularMaterialData), NULL);
    m_uniformBufferMgr.bindUniformBuffer(
        m_materialUniformBufferIndex, m_sceneShader.GetShaderProgram(), "material");
    ///

    return status;
}

void Scene::drawScene()
{
    Camera* activeCamPtr = m_pCameraList[m_activeCamera];

#if 0
    Vector3 camPos = activeCamPtr->GetTrans().GetPos();
    printf("%f %f %f\n", camPos.x, camPos.y, camPos.z);
#endif

    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint sceneShaderProgram;
    GLuint materialIndex;

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        Model* pModel = m_pSceneModelList[i];

        MaterialType modelMaterialType = pModel->GetModelMaterialType();

        switch (modelMaterialType)
        {
        case MaterialType::PHONG:
            sceneShaderProgram = m_sceneShader.useProgram();
            materialIndex      = m_materialUniformBufferIndex;
            break;
        case MaterialType::COOKTORRANCE:
            sceneShaderProgram = m_pbrShader.useProgram();
            materialIndex      = m_pbrMaterialUniformBufferIndex;
            break;
        default:
            printf("Unsupport material!\n");
            assert(FALSE);
            break;
        }

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

        GLint eyeHandle = glGetUniformLocation(sceneShaderProgram, "eyePos");
        glUniform3fv(eyeHandle, 1, glm::value_ptr(activeCamPtr->GetTrans().GetPos()));

        m_pTextureList[0]->bindTexture(GL_TEXTURE0, sceneShaderProgram, "sampler", 0);

        glm::mat4 lightTransWVP  = m_pShadowMap->GetLightTransVP() *
                                   pModel->m_pTrans->GetTransMatrix();

        GLint lightTransHandle = glGetUniformLocation(sceneShaderProgram, "lightTransWVP");
        glUniformMatrix4fv(lightTransHandle, 1, GL_FALSE, glm::value_ptr(lightTransWVP));

        m_pShadowMap->readShadowMap(GL_TEXTURE1, sceneShaderProgram, "shadowMapSampler", 1);

        pModel->updateMaterial(&m_uniformBufferMgr, materialIndex);
        pModel->draw();
    }

    Shader::FinishProgram();
}

BOOL Scene::initializeDeferredRendering()
{
    BOOL status = TRUE;

    // G-Buffer shader
    m_defferedRendingShader.setShaderFiles("MainSceneDefferedDraw.vert", "MainSceneDefferedDraw.frag");
    m_defferedRendingShader.linkProgram();

    m_pGBuffer = new GBuffer(this, m_setting.width, m_setting.height);
    status = m_pGBuffer->initialize();

    if (status == FALSE)
    {
        Shader::AssertErrors();
    }

    m_uniformBufferMgr.bindUniformBuffer(
        m_directionalLightUniformBufferIndex,
        m_defferedRendingShader.GetShaderProgram(),
        "directionalLightUniformBlock");

    return status;
}

void Scene::deferredDrawScene()
{
    assert(m_setting.m_graphicsSetting.renderingMethod == RenderingMethod::DEFERRED_RENDERING);

    GLuint gShaderProgram = m_defferedRendingShader.useProgram();
    m_pGBuffer->readGBuffer(gShaderProgram);

    Camera* activeCamPtr = m_pCameraList[m_activeCamera];
    
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint eyeLocation = glGetUniformLocation(gShaderProgram, "eyePos");

    if (eyeLocation >= 0)
    {
        glUniform3fv(eyeLocation, 1, glm::value_ptr(activeCamPtr->GetTrans().GetPos()));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_pGBuffer->draw();
        glDisable(GL_BLEND);
    }
    else
    {
        printf("Unable to get wvp matrix location in shadowMap shader");
        assert(FALSE);
    }    

    Shader::FinishProgram();
}

BOOL Scene::initializePBRendering()
{
    BOOL status = TRUE;

    // PBR shader
    m_pbrShader.setShaderFiles("MainScene.vert", "MainSceneCookTorrance.frag");
    status = m_pbrShader.linkProgram();

    if (status == FALSE)
    {
        Shader::AssertErrors();
    }

    m_uniformBufferMgr.bindUniformBuffer(
        m_transUniformbufferIndex,
        m_pbrShader.GetShaderProgram(),
        "transUniformBlock");

    // Directional light ubo
    m_uniformBufferMgr.bindUniformBuffer(
        m_directionalLightUniformBufferIndex,
        m_pbrShader.GetShaderProgram(),
        "directionalLightUniformBlock");

    // Point light ubo
    m_uniformBufferMgr.bindUniformBuffer(
        m_pointLightUniformBufferIndex,
        m_pbrShader.GetShaderProgram(),
        "pointLightUniformBlock");

    m_pbrMaterialUniformBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(
            GL_DYNAMIC_DRAW, CookTorranceMaterial::GetOpaqueCookTorranceMaterialDataSize(), NULL);
    m_uniformBufferMgr.bindUniformBuffer(
        m_pbrMaterialUniformBufferIndex, m_pbrShader.GetShaderProgram(), "CookTorranceMaterialUniformBlock");

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
