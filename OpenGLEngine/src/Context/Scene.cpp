#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

//Image loader
#include "stb_image.h"

Scene::Scene(const Setting& setting)
    : m_backgroundColor(0.0f, 0.0f, 0.6f, 1.0f),
      m_directionalLight(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
      m_activeCamera(0), m_uniformBufferMgr(128)
{
    this->setting = setting;
}

BOOL Scene::initialize()
{
    addCamera(CameraType::PROJECT_CAM, glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0, 0, 0),
              glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height);

    addCamera(CameraType::PROJECT_CAM, glm::vec3(-5.0f, 2.0f, 2.0f), glm::vec3(0, 0, 0),
              glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height);

    Vector3 lightDir = m_directionalLight.getDir();

    glm::vec3 glmLightDir = glm::vec3(lightDir.x, lightDir.y, lightDir.z);
    glm::vec3 lightPos = glm::vec3(0.0f, 2.5f, 0.0f);
    m_pLightCamera = new ProspectiveCamera(
        lightPos, lightPos + glmLightDir, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, (float)setting.width / (float)setting.height);

    //Load model and texture(Hardcode here)
    Transform* pTrans = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
    addModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl", pTrans);

    /*addModel("../../resources/models/cornellbox/CornellBox-Sphere.obj",
             "../../resources/models/cornellbox/CornellBox-Sphere.mtl", pTrans);*/
    //addModel("../resources/models/sphere/sphere.obj", "", pTrans);

    //Transform* pTrans2 = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
    //addModel("../../resources/models/sphere/sphere.obj", "", pTrans);
        
    //Create texture and set sampler
    m_pTextureList.push_back(new Texture2D("../resources/textures/teaport/wall.jpg", 
                             GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_TRUE));

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
    m_uniformBufferMgr.bindUniformBuffer(m_transUniformbufferIndex, m_sceneShader.GetShaderProgram(), "trans");

    m_lightUniformBufferIndex = 
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW,
                                               m_directionalLight.getDataSize(),
                                               m_directionalLight.getDataPtr());
    m_uniformBufferMgr.bindUniformBuffer(m_lightUniformBufferIndex, m_sceneShader.GetShaderProgram(), "lightUniformBlock");

    m_materialBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW, sizeof(SpecularMaterial::m_materialData), NULL);
    m_uniformBufferMgr.bindUniformBuffer(m_materialBufferIndex, m_sceneShader.GetShaderProgram(), "material");

    // Shadow map test
    m_shadowMap.createFramebuffer(setting.width, setting.height);

    m_shadowMapShader.setShaderFiles("ShadowMap.vert", "ShadowMap.frag");
    hs = m_shadowMapShader.linkProgram();
    if (hs == FALSE)
    {
        assert("Fail to compile shadow map shaders.\n");
        return FALSE;
    }

    return TRUE;
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

    drawPass();
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
        case CameraType::PROJECT_CAM:
            SafeDelete(static_cast<ProspectiveCamera*>(pCamera));
            break;
        case CameraType::ORTHO_CAM:
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
}

void Scene::setSceneShader(
    char* const vertexShaderFile,
    char* const fragmentShaderFile)
{
    m_sceneShader.setShaderFiles(vertexShaderFile, fragmentShaderFile);
}

void Scene::shadowPass()
{
    if (m_pLightCamera != NULL)
    {
        m_shadowMapShader.useProgram();
        m_shadowMap.drawFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
        {
            glm::mat4 worldMatrix    = glm::translate(glm::mat4(), m_pSceneModelList[i]->trans->pos);
            glm::mat4 viewMatrix     = m_pLightCamera->getViewMatrix();
            glm::mat4 prospectMatrix = m_pLightCamera->getProjectionMatrix();

            glm::mat4 wvp = prospectMatrix * viewMatrix * worldMatrix;

            GLint tranMatrixLocation = glGetUniformLocation(m_shadowMapShader.GetShaderProgram(), "wvp");
            glUniformMatrix4fv(tranMatrixLocation, 1, GL_FALSE, glm::value_ptr(wvp));

            m_pSceneModelList[i]->drawModelPos();
        }

        m_shadowMap.finishDrawFramebuffer();
    }
    else
    {
        assert("No light camera, can't cast shadow.");
    }
}

void Scene::drawPass()
{
    m_sceneShader.useProgram();

    ProspectiveCamera* activeCamPtr = static_cast<ProspectiveCamera*>(m_pCameraList[m_activeCamera]);

#if 0
    Vector3 camPos = activeCamPtr->getTrans().pos;
    printf("%f %f %f\n", camPos.x, camPos.y, camPos.z);
#endif

    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_uniformBufferMgr.
        updateUniformBufferData(
            m_lightUniformBufferIndex,
            m_directionalLight.getDataSize(),
            m_directionalLight.getDataPtr());

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        glm::mat4 transMatrix[4] =
        {
            glm::translate(glm::mat4(), m_pSceneModelList[i]->trans->pos),
            activeCamPtr->getViewMatrix(),
            activeCamPtr->getProjectionMatrix(),
            glm::mat4()
        };
        transMatrix[3] = transMatrix[2] * transMatrix[1] * transMatrix[0];

        m_uniformBufferMgr.updateUniformBufferData(
            m_transUniformbufferIndex, sizeof(transMatrix), &(transMatrix[0]));

        //m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(10.0f));

        GLint eyeHandle = glGetUniformLocation(m_sceneShader.GetShaderProgram(), "eyePos");
        glUniform3fv(eyeHandle, 1, glm::value_ptr(activeCamPtr->getTrans().pos));

        m_pTextureList[0]->bindTexture(GL_TEXTURE0, m_sceneShader.GetShaderProgram(), "sampler", 0);

        glm::mat4 lightTranslate = glm::translate(glm::mat4(), m_pLightCamera->getTrans().pos);
        glm::mat4 lightTransWVP  = m_pLightCamera->getProjectionMatrix() *
                                   m_pLightCamera->getViewMatrix()       *
                                   lightTranslate;

        GLint lightTransHandle = glGetUniformLocation(m_sceneShader.GetShaderProgram(), "lightTransWVP");
        glUniformMatrix4fv(lightTransHandle, 1, GL_FALSE, glm::value_ptr(lightTransWVP));

        m_shadowMap.getTexturePtr(GL_TEXTURE0)->
            bindTexture(GL_TEXTURE1, m_sceneShader.GetShaderProgram(), "shadowMapSampler", 1);

        m_pSceneModelList[i]->updateMaterial(&m_uniformBufferMgr, m_materialBufferIndex);
        m_pSceneModelList[i]->draw();
    }
}

void Scene::addModel(
    const std::string& modelFile,
    const std::string& materialFile,
    Transform*         modelTrans)
{
    Model* pModel = new Model(modelFile, materialFile, modelTrans);
    m_pSceneModelList.push_back(pModel);
}

void Scene::addCamera(
    const CameraType type,
    const glm::vec3& pos,
    const glm::vec3& center,
    const glm::vec3& up,
    float speed,
    float aspectRatio,
    float nearClip,
    float farClip,
    float fov)
{
    switch (type)
    {
    case CameraType::PROJECT_CAM:
    {
        ProspectiveCamera * pProspectiveCamera =
            new ProspectiveCamera(pos, center, up, speed, aspectRatio, nearClip, farClip, fov);

        m_pCameraList.push_back(pProspectiveCamera);

        break;
    }
    case CameraType::ORTHO_CAM:
        // TODO
        break;
    default:
        assert("Invalid camera type!");
        break;
    }
}
