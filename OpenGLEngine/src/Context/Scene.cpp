#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

//Image loader
#include "stb_image.h"

Scene::Scene(const Setting& setting)
    : m_backgroundColor(0.0f, 0.0f, 0.6f, 1.0f),
      m_directionalLight(Vector3(1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
      m_pointLight(Vector3(0.0f, 5.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 10.0f),
      m_activeCamera(0),
      m_uniformBufferMgr(128)
{
    this->setting = setting;
}

BOOL Scene::initialize()
{
    //Compile shaders
    m_sceneShader.setShaderFiles("MainSceneShadowMap.vert", "MainSceneShadowMap.frag");
    BOOL hs = m_sceneShader.linkProgram();

    if (hs == FALSE)
    {
        assert("Fail to compile shaders.\n");
        return FALSE;
    }

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
    m_materialBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(
            GL_DYNAMIC_DRAW, sizeof(SpecularMaterial::m_materialData), NULL);
    m_uniformBufferMgr.bindUniformBuffer(
        m_materialBufferIndex, m_sceneShader.GetShaderProgram(), "material");

    // Shadow map test
    m_pShadowMap = new ShadowMap(
       static_cast<Light*>(&m_directionalLight), setting.width, setting.height, setting.m_graphicsSetting.antialasing);
    hs = m_pShadowMap->initialize();

    // Deferred shading
    if (hs == TRUE && setting.m_graphicsSetting.shadingMethod == RenderingMethod::DEFERRED_RENDERING)
    {
        m_pGBuffer = new GBuffer(setting.width, setting.height);
        hs = m_pGBuffer->initialize();
    }

    return hs;
}

void Scene::update(float deltaTime)
{
    UINT cameraCount = m_pCameraList.size();

    assert(cameraCount > 0);

    if (callbackInfo.keyboardCallBack[GLFW_KEY_1] == 1)
    {
        m_activeCamera = 0;
    }
    else if (callbackInfo.keyboardCallBack[GLFW_KEY_2] == 1)
    {
        m_activeCamera = ((1 < cameraCount) ? 1 : m_activeCamera);
    }
    else if (callbackInfo.keyboardCallBack[GLFW_KEY_3] == 1)
    {
        m_activeCamera = ((2 < cameraCount) ? 2 : m_activeCamera);
    }
    else if (callbackInfo.keyboardCallBack[GLFW_KEY_4] == 1)
    {
        m_activeCamera = ((3 < cameraCount) ? 3 : m_activeCamera);
    }

    m_pCameraList[m_activeCamera]->update(deltaTime);
}

void Scene::draw()
{
    GLfloat timeValue = static_cast<GLfloat>(glfwGetTime());

    shadowPass();

    if (setting.m_graphicsSetting.shadingMethod == RenderingMethod::FORWARD_RENDERING)
    {
        drawScene();
    }
    else
    {
        m_pGBuffer->drawGBuffer(this);
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
            assert("Invalid camera type");

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
            assert("Invalid texture type");

            SafeDelete(pTexture);
            break;
        }
    }
    m_pTextureList.clear();

    //SafeDelete(m_pLightCamera);
    //SafeDelete(m_pDirectionalLightCamera);

    SafeDelete(m_pShadowMap);
    SafeDelete(m_pGBuffer);
}

void Scene::setSceneShader(
    char* const vertexShaderFile,
    char* const fragmentShaderFile)
{
    m_sceneShader.setShaderFiles(vertexShaderFile, fragmentShaderFile);
}

void Scene::shadowPass()
{
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
    Vector3 camPos = activeCamPtr->getTrans().pos;
    printf("%f %f %f\n", camPos.x, camPos.y, camPos.z);
#endif

    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_uniformBufferMgr.
        updateUniformBufferData(
            m_directionalLightUniformBufferIndex,
            m_directionalLight.getDataSize(),
            m_directionalLight.getDataPtr());

    m_uniformBufferMgr.
        updateUniformBufferData(
            m_pointLightUniformBufferIndex,
            m_pointLight.getDataSize(),
            m_pointLight.getDataPtr());

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        glm::mat4 transMatrix[4] =
        {
            m_pSceneModelList[i]->m_pTrans->GetTransMatrix(),
            activeCamPtr->GetViewMatrix(),
            activeCamPtr->GetProjectionMatrix(),
            glm::mat4()
        };
        transMatrix[3] = transMatrix[2] * transMatrix[1] * transMatrix[0];

        m_uniformBufferMgr.updateUniformBufferData(
            m_transUniformbufferIndex, sizeof(transMatrix), &(transMatrix[0]));

        //m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(10.0f));

        GLint eyeHandle = glGetUniformLocation(m_sceneShader.GetShaderProgram(), "eyePos");
        glUniform3fv(eyeHandle, 1, glm::value_ptr(activeCamPtr->getTrans().GetPos()));

        m_pTextureList[0]->bindTexture(GL_TEXTURE0, m_sceneShader.GetShaderProgram(), "sampler", 0);

        glm::mat4 lightTransWVP  = m_pShadowMap->GetLightTransVP() *
                                   m_pSceneModelList[i]->m_pTrans->GetTransMatrix();

        GLint lightTransHandle = glGetUniformLocation(m_sceneShader.GetShaderProgram(), "lightTransWVP");
        glUniformMatrix4fv(lightTransHandle, 1, GL_FALSE, glm::value_ptr(lightTransWVP));

        m_pShadowMap->readShadowMap(GL_TEXTURE1, m_sceneShader.GetShaderProgram(), "shadowMapSampler", 1);

        m_pSceneModelList[i]->updateMaterial(&m_uniformBufferMgr, m_materialBufferIndex);
        m_pSceneModelList[i]->draw();
    }

    m_sceneShader.finishProgram();
}

void Scene::deferredDrawScene()
{
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
        assert("Unsupport texture type!");
        break;
    }
}
