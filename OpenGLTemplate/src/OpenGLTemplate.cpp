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
    m_context.initialize();

    Scene* pScene = m_context.GetScene();
    pScene->EnableDebug();

    pScene->AddDirectionalLight(Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.5f, 0.5f, 0.5f));
    pScene->AddDirectionalLight(Vector3( 1.0f, -1.0f, -1.0f), Vector3(0.5f, 0.5f, 0.5f));

    pScene->AddSpotLight(
        Vector3(0.0f, 3.0f, 2.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.5f, 0.0f, 0.0f), 5.0f,
        Math::Radians(60.0f), Math::Radians(90.0f));

    pScene->AddSpotLight(
        Vector3(2.0f, 3.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.5f, 0.0f), 5.0f,
        Math::Radians(60.0f), Math::Radians(90.0f));

    pScene->AddSpotLight(
        Vector3(-2.0f, 3.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 0.5f), 5.0f,
        Math::Radians(60.0f), Math::Radians(90.0f));

    // pScene->AddPointLight(Vector3(0.0f, 5.0f, 0.0f), Vector3(0.5f, 0.5f, 0.5f), 10.0f);

    /*pScene->AddPointLight(Vector3(0.0f, 5.0f, -5.0f), Vector3(0.5f, 0.0f, 0.0f), 10.0f);
    pScene->AddPointLight(Vector3(5.0f, 5.0f, 0.0f), Vector3(0.0f, 0.5f, 0.0f), 10.0f);
    pScene->AddPointLight(Vector3(-5.0f, 5.0f, 0.0f), Vector3(0.0f, 0.0f, 0.5f), 10.0f);*/

    /*pScene->AddDirectionalLight(Vector3( 1.0f, -1.0f, -1.0f), Vector3(0.5f, 0.0f, 0.0f));
    pScene->AddDirectionalLight(Vector3(-1.0f, -1.0f,  1.0f), Vector3(0.0f, 0.5f, 0.0f));
    pScene->AddDirectionalLight(Vector3( 1.0f, -1.0f,  1.0f), Vector3(0.0f, 0.0f, 0.5f));*/

    const float aspectRadio = static_cast<float>(m_pSetting->resolution.width) /
                              static_cast<float>(m_pSetting->resolution.height);

    /*pScene->addProspectiveCamera(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0, 0, 1.0f), 5.0f, aspectRadio, 0.1f, 1000.0f, 60.0f);*/

    /*float halfWidth  = static_cast<float>(m_pSetting->resolution.width * 2) * 0.002f;
    float halfHeight = static_cast<float>(m_pSetting->resolution.width * 2) * 0.002f;*/

    //Vector3 lightDir = pScene->m_directionalLight.getDir();
    //glm::vec3 glmLightDir = glm::vec3(lightDir.x, lightDir.y, lightDir.z);

    //float lightPosScale = 100.0f;
    /*pScene->addOrthographicCamera(
        -glmLightDir * lightPosScale, glmLightDir, glm::vec3(0, 1, 0),
        5.0f, Rectangle(-halfWidth, halfWidth, -halfHeight, halfHeight), 0.1f, 1000.0f);*/

    /*pScene->addProspectiveCamera(glm::vec3(0.0f, 15.0f, 10.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);*/

    /*pScene->addProspectiveCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f, 70.0f);*/

    //pScene->AddDirectionalLight(Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.5f, 0.5f, 0.5f));

    pScene->addProspectiveCamera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f, 70.0f);

    /*pScene->addProspectiveCamera(glm::vec3(0.0f, 5.0f, 0.1f), glm::vec3(0, 4, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f, 70.0f);*/

    //m_pLightCamera = new ProspectiveCamera(
    //    /*lightPos, lightPos + glmLightDir,*/
    //    glm::vec3(0.0f, 10.0f, 0.1f), glm::vec3(0, 4, 0),
    //    glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, aspectRadio, 0.1f, 1000.0f);

    //Load model and texture(Hardcode here)
    /*pScene->addModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl",
    new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
    new Trans(glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/
    
    //Model* pModel = pScene->GetModelPtr(0);

    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3( 3.0f, 0.0f,  0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    /*pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3( 0.0f, 0.0f,  3.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3( 3.0f, 0.0f,  3.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3( 3.0f, 0.0f, -3.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3(-3.0f, 0.0f,  0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3(-3.0f, 0.0f,  3.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3( 0.0f, 0.0f, -3.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
        new Trans(glm::vec3(-3.0f, 0.0f, -3.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    pScene->addModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl",
        new Trans(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));
    pScene->addModel("../resources/models/plane/plane.obj", "../resources/models/plane/plane.mtl",
        new Trans(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));

    /*pScene->addModel(
        "../resources/models/cornellbox/CornellBox-Sphere.obj",
        "../resources/models/cornellbox/CornellBox-Sphere.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    //pScene->GetModelPtr(0)->m_pTrans->SetScale(glm::vec3(5.0f, 5.0f, 5.0f));

    /*pScene->addModel(
        "../resources/models/buddha/buddha.obj",
        "../resources/models/buddha/buddha.mtl",
        new Trans(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    /*pScene->addModel(
        "../resources/models/dragon/dragon.obj",
        "../resources/models/dragon/dragon.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    //Create texture and set sampler
    pScene->addTexture("../resources/textures/teaport/wall.jpg",
        GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_TRUE);

    pScene->addSkyboxImage(
        "../resources/textures/skybox/SunSet/SunSetLeft2048.png",
        "../resources/textures/skybox/SunSet/SunSetRight2048.png",
        "../resources/textures/skybox/SunSet/SunSetUp2048.png",
        "../resources/textures/skybox/SunSet/SunSetDown2048.png",
        "../resources/textures/skybox/SunSet/SunSetFront2048.png",
        "../resources/textures/skybox/SunSet/SunSetBack2048.png");

    pScene->disableSceneLocalMaterial();
}
