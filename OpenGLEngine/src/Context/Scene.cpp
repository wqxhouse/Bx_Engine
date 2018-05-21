#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

//Image loader
#include "stb_image.h"

Scene::Scene(const Setting & setting)
    : m_backgroundColor(0.0f, 0.0f, 0.6f, 1.0f),
      m_directionalLight(Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f)),
      m_activeCamera(0), m_uniformBufferMgr(128)
{
    this->setting = setting;
}

BOOL Scene::initialize()
{
    m_shadowMap.createFramebuffer(setting.width, setting.height);

    glGenBuffers(1, &transformHandle);

    addCamera(CameraType::PROJECT_CAM, glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0, 0, 0),
              glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height);

    addCamera(CameraType::PROJECT_CAM, glm::vec3(-5.0f, 2.0f, 2.0f), glm::vec3(0, 0, 0),
              glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height);

    //Load model and texture(Hardcode here)
    Transform* pTrans = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
    addModel("../../resources/models/cornellbox/CornellBox-Sphere.obj",
             "../../resources/models/cornellbox/CornellBox-Sphere.mtl", pTrans);
    //addModel("../resources/models/sphere/sphere.obj", pTrans);

    //Transform* pTrans2 = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
    //addModel("../../resources/models/sphere/sphere.obj", "", pTrans);

    
    //Create texture and set sampler
    m_pTextureList.push_back(new Texture2D("../resources/textures/teaport/wall.jpg", 
                             GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_TRUE));

    //Compile shaders
    ShaderCompiler m_shaderCompiler;
    const char* vertexShaderFile   = "SimpleTexture.vert";
    const char* fragmentShaderFile = "SimpleTexture.frag";
    int hs = m_shaderCompiler.compileShader(vertexShaderFile, fragmentShaderFile, &simpleTextureProgram);

    if (hs != 0)
    {
        assert("Fail to compile shaders.\n");
        return FALSE;
    }

    m_transUniformbufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW, sizeof(Mat4) * 4, nullptr);
    m_uniformBufferMgr.bindUniformBuffer(m_transUniformbufferIndex, simpleTextureProgram, "trans");

    m_lightUniformBufferIndex = 
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW,
                                               m_directionalLight.getDataSize(),
                                               m_directionalLight.getDataPtr());
    m_uniformBufferMgr.bindUniformBuffer(m_lightUniformBufferIndex, simpleTextureProgram, "light");

    SpecularMaterial* pMaterial =
        static_cast<SpecularMaterial*>(m_pSceneModelList[0]->m_pMeshList[1]->m_pMaterial);
    m_materialBufferIndex =
        m_uniformBufferMgr.createUniformBuffer(GL_DYNAMIC_DRAW,
                                               sizeof(pMaterial->m_materialData),
                                               NULL);
    m_uniformBufferMgr.bindUniformBuffer(m_materialBufferIndex, simpleTextureProgram, "material");

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
    glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(simpleTextureProgram);

    GLfloat timeValue = (GLfloat)glfwGetTime();
    GLfloat colorValue = 1.0f;
    GLint glVertexColorLocation = glGetUniformLocation(simpleTextureProgram, "uniformColor");
    glUniform3f(glVertexColorLocation, colorValue, colorValue, colorValue);

    ProspectiveCamera* activeCamPtr = static_cast<ProspectiveCamera*>(m_pCameraList[m_activeCamera]);

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        glm::mat4 transMatrixs[4] =
        {
            glm::translate(glm::mat4(), m_pSceneModelList[i]->trans->pos),
            activeCamPtr->getViewMatrix(),
            activeCamPtr->getProjectionMatrix(),
            glm::mat4()
        };
        transMatrixs[3] = transMatrixs[2] * transMatrixs[1] * transMatrixs[0];

        m_uniformBufferMgr.updateUniformBufferData(
            m_transUniformbufferIndex, sizeof(transMatrixs), &(transMatrixs[0]));
        
        //m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(10.0f));

        m_uniformBufferMgr.
            updateUniformBufferData(
                m_lightUniformBufferIndex,
                m_directionalLight.getDataSize(),
                m_directionalLight.getDataPtr());

        GLint glEyeHandle = glGetUniformLocation(simpleTextureProgram, "eyePos");
        glUniform3fv(glEyeHandle, 1, glm::value_ptr(activeCamPtr->getTrans().pos));

        m_pTextureList[0]->bindTexture(GL_TEXTURE0, simpleTextureProgram, "sampler", 0);

        m_pSceneModelList[i]->updateMaterial(&m_uniformBufferMgr, m_materialBufferIndex);
        m_pSceneModelList[i]->draw();
    }
}

Scene::~Scene()
{
    glDeleteBuffers(1, &transformBuffer);
    glDeleteBuffers(1, &normalTransformBuffer);
    glDeleteProgram(simpleTextureProgram);

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
