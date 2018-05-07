#include "Scene.h"

#include "../Model/Mesh/ObjModelLoader.h"
#include "../Core/Utility.h"

//Image loader
#include "stb_image.h"

Scene::Scene(const Setting & setting)
    : m_directionalLight(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)),
      m_activeCamera(0)
{
    this->setting = setting;
}

int Scene::initialize()
{
    m_pCameraList.push_back(
        new ProspectiveCamera(glm::vec3(5.0f, 2.0f, 2.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height));

    m_pCameraList.push_back(
        new ProspectiveCamera(glm::vec3(-5.0f, 2.0f, 2.0f), glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0), 5.0f, (float)setting.width / (float)setting.height));

    ////Load model and texture(Hardcode here)
    //ObjModelLoader objLoader;
    //objLoader.LoadModel("../resources/models/farmhouse/farmhouse_tri.obj", &(pModel->mesh));
    Transform* pTrans = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
    addModel("../../resources/models/cornellbox/CornellBox-Sphere.obj",
             "../../resources/models/cornellbox/CornellBox-Sphere.mtl", pTrans);
    //addModel("../resources/models/sphere/sphere.obj", pTrans);

    //Transform* pTrans2 = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
    //addModel("../../resources/models/sphere/sphere.obj", "", pTrans);

    //glGenBuffers(1, &lightParagBuffer);
    glGenBuffers(1, &transformHandle);
    
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
        return -1;
    }

    /*transformHandle = glGetUniformBlockIndex(simpleTextureProgram, "trans");
    glGetActiveUniformBlockiv(simpleTextureProgram, transformHandle, GL_UNIFORM_BLOCK_DATA_SIZE, &transformBufferSize);
    transformBufferData = (GLubyte*)malloc(transformBufferSize);

    glBindBuffer(GL_UNIFORM_BUFFER, transformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, transformBufferSize, transformBufferData, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, transformHandle, transformBuffer);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);*/

    lightData[0] = { "lightDir",   sizeof(glm::vec3), static_cast<void*>(&(m_directionalLight.getDir())) };
    lightData[1] = { "lightColor", sizeof(glm::vec3), static_cast<void*>(&(m_directionalLight.getLightColor())) };

    m_lightBuffer = new UniformBuffer(simpleTextureProgram, "light", 2, lightData, GL_DYNAMIC_DRAW);

    return 0;
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
    glUseProgram(simpleTextureProgram);

    GLfloat timeValue = (GLfloat)glfwGetTime();
    GLfloat colorValue = 1.0f;// (GLfloat)(sin(timeValue) / 2) + 0.5;
    GLint glVertexColorLocation = glGetUniformLocation(simpleTextureProgram, "uniformColor");
    glUniform3f(glVertexColorLocation, colorValue, colorValue, colorValue);

    ProspectiveCamera* activeCamPtr = static_cast<ProspectiveCamera*>(m_pCameraList[m_activeCamera]);

    for (size_t i = 0; i < m_pSceneModelList.size(); ++i)
    {
        //m_directionalLight.rotate(Vector3(0.0f, 1.0f, 0.0f), glm::radians(10.0f));
        lightData[0].data = static_cast<void*>(&(m_directionalLight.getDir()));
        lightData[1].data = static_cast<void*>(&(m_directionalLight.getLightColor()));
        m_lightBuffer->update(lightData);

        glm::mat4 rotation;
        //rotation = glm::rotate(rotation, glm::radians(180.0f) * timeValue, glm::vec3(0, 1, 0));

        GLint glRotationMatHandle = glGetUniformLocation(simpleTextureProgram, "rot");
        glUniformMatrix4fv(glRotationMatHandle, 1, GL_FALSE, glm::value_ptr(rotation));

        glm::mat4 transform;
        transform = glm::translate(transform, m_pSceneModelList[i]->trans->pos/*glm::vec3(10.0f, 10.0f, 10.0f)*/);
        //transform *= rotation; //glm::rotate(transform, glm::radians(180.0f) * timeValue, glm::vec3(0, 1, 0));
        //transform = glm::scale(transform, glm::vec3(.5f, .5f, .5f));

        GLint glWorldMatrixLocation = glGetUniformLocation(simpleTextureProgram, "world");
        glUniformMatrix4fv(glWorldMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));

        //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0, 1, 0));
        GLint glViewMatrixLocation = glGetUniformLocation(simpleTextureProgram, "view");
        glUniformMatrix4fv(glViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(activeCamPtr->getViewMatrix()/*view*/));

        GLint glProjectionLocation = glGetUniformLocation(simpleTextureProgram, "proj");
        glUniformMatrix4fv(glProjectionLocation, 1, GL_FALSE, glm::value_ptr(activeCamPtr->getProjectionMatrix()));

       // printf("%f %f %f\n", activeCamPtr->getTrans().pos.x, activeCamPtr->getTrans().pos.y, activeCamPtr->getTrans().pos.z);
        GLint glEyeHandle = glGetUniformLocation(simpleTextureProgram, "eyePos");
        glUniform3fv(glEyeHandle, 1, glm::value_ptr(activeCamPtr->getTrans().pos));

        m_pTextureList[0]->bindTexture(GL_TEXTURE0, simpleTextureProgram, "sampler", 0);

        m_pSceneModelList[i]->draw();
    }
}

Scene::~Scene()
{
    glDeleteBuffers(1, &transformBuffer);
    glDeleteBuffers(1, &normalTransformBuffer);
    //glDeleteBuffers(1, &lightParagBuffer);
    glDeleteProgram(simpleTextureProgram);

    SafeDelete(m_lightBuffer);

    /*SafeFree(lightDataBuffer);
    SafeFree(transformBufferData);
    SafeFree(normalTransformBufferData);*/

    for (Model* model : m_pSceneModelList)
    {
        delete(model);
    }
    m_pSceneModelList.clear();

    for (Camera* pCamera : m_pCameraList)
    {
        switch (pCamera->m_type)
        {
        case Camera::CameraType::PROJECT_CAM:
            delete static_cast<ProspectiveCamera*>(pCamera);
            break;
        case Camera::CameraType::ORTHO_CAM:
            delete static_cast<OrthographicCamera*>(pCamera);
            break;
        default:
            break;
        }
    }
}

void Scene::addModel(const std::string & modelFile, const std::string & materialFile, Transform * modelTrans)
{
    Model* pModel = new Model(modelFile, materialFile, modelTrans);
    m_pSceneModelList.push_back(pModel);
}
