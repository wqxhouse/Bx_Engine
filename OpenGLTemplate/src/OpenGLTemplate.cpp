#include "OpenGLTemplate.h"

OpenGLTemplate::OpenGLTemplate(
    Setting*       pSetting,
    const BOOL     defaultScene)
    : m_pSetting(pSetting),
      m_context(pSetting)
{
    if (defaultScene == TRUE)
    {
        createDefaultScene();
    }
}

OpenGLTemplate::~OpenGLTemplate()
{
}

void OpenGLTemplate::run()
{
    m_context.run();
}

void OpenGLTemplate::createDefaultScene()
{
    Scene* pScene = m_context.GetScene();

    const float aspectRadio = static_cast<float>(m_pSetting->width) / static_cast<float>(m_pSetting->height);

    float halfWidth = static_cast<float>(m_pSetting->width * 2) * 0.002f;
    float halfHeight = static_cast<float>(m_pSetting->width * 2) * 0.002f;

    //Vector3 lightDir = pScene->m_directionalLight.getDir();
    //glm::vec3 glmLightDir = glm::vec3(lightDir.x, lightDir.y, lightDir.z);

    //float lightPosScale = 100.0f;
    /*pScene->addOrthographicCamera(
        -glmLightDir * lightPosScale, glmLightDir, glm::vec3(0, 1, 0),
        5.0f, Rectangle(-halfWidth, halfWidth, -halfHeight, halfHeight), 0.1f, 1000.0f);*/

    /*pScene->addProspectiveCamera(glm::vec3(0.0f, 15.0f, 10.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);*/

    pScene->addProspectiveCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);

    pScene->addProspectiveCamera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);

    pScene->addProspectiveCamera(glm::vec3(0.0f, 5.0f, 0.1f), glm::vec3(0, 4, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);

    //m_pLightCamera = new ProspectiveCamera(
    //    /*lightPos, lightPos + glmLightDir,*/
    //    glm::vec3(0.0f, 10.0f, 0.1f), glm::vec3(0, 4, 0),
    //    glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, aspectRadio, 0.1f, 1000.0f);

    //Load model and texture(Hardcode here)
    /*addModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl",
    new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
    new Trans(glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    addModel("../resources/models/plane/plane.obj", "../resources/models/plane/plane.mtl",
    new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    */
    /*pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/
    //Model* pModel = pScene->GetModelPtr(0);

    /*pScene->addModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    /*pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    pScene->addModel(
        "../resources/models/cornellbox/CornellBox-Sphere.obj",
        "../resources/models/cornellbox/CornellBox-Sphere.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));

    //pScene->GetModelPtr(0)->m_pTrans->SetScale(glm::vec3(5.0f, 5.0f, 5.0f));

    /*pScene->addModel(
        "../resources/models/buddha/buddha.obj",
        "../resources/models/buddha/buddha.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    /*pScene->addModel(
        "../resources/models/dragon/dragon.obj",
        "../resources/models/dragon/dragon.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    //Create texture and set sampler
    pScene->addTexture("../resources/textures/teaport/wall.jpg",
        GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_TRUE);

    pScene->disableSceneLocalMaterial();
}
