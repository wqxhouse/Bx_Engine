#include "MeshlessDeformation.h"

MeshlessDeformation::MeshlessDeformation()
    : m_context(&m_setting)
{
}

MeshlessDeformation::~MeshlessDeformation()
{
    SafeDelete(m_pPhysicsSys);
}

void MeshlessDeformation::init()
{
    initScene();

    pScene  = m_context.GetScene();

    m_pMesh = pScene->GetModelPtr(0)->GetMesh(0);

    m_meshDataCount = m_pMesh->m_vertexBuffer.size();
    m_meshDataCountInv = 1.0f / static_cast<float>(m_meshDataCount);

    m_pPhysicsSys = new PhysicsSystem(m_meshDataCount);

    // Pre-compute geometry center t0
    for (const Mesh::Vertex v : m_pMesh->m_vertexBuffer)
    {
        t0 += v.position;
    }

    t0 *= m_meshDataCountInv;

    // Pre-compute q and Aqq
    q.resize(m_meshDataCount);
    
    for (UINT i = 0; i < m_meshDataCount; ++i)
    {
        q[i] = m_pMesh->m_vertexBuffer[i].position - t0;

        Aqq += Math::Mat3(q[i].x * q[i].x, q[i].x * q[i].y, q[i].x * q[i].z,
                          q[i].y * q[i].x, q[i].y * q[i].y, q[i].y * q[i].z,
                          q[i].z * q[i].x, q[i].z * q[i].y, q[i].z * q[i].z);
    }

    // Inverse origin Aqq matrix
    Aqq.inverse();

    // Resize p vector
    p.resize(m_meshDataCount);
}

void MeshlessDeformation::initScene()
{
    Scene* pScene = m_context.GetScene();
    GLenum err = glGetError();
    const float aspectRadio = static_cast<float>(m_setting.resolution.width) /
                              static_cast<float>(m_setting.resolution.height);

    pScene->addProspectiveCamera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);

    pScene->addProspectiveCamera(glm::vec3(0.0f, 5.0f, 0.1f), glm::vec3(0, 4, 0),
        glm::vec3(0, 1, 0), 5.0f, aspectRadio, 0.1f, 1000.0f);

    /*pScene->addModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl",
        new Trans(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    /*pScene->addModel("../resources/models/plane/plane.obj", "../resources/models/plane/plane.mtl",
        new Trans(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f)));*/

    Model* pModel = pScene->GetModelPtr(0);
    Mesh* pMesh = pModel->GetMesh(0);

    pScene->addTexture("../resources/textures/teaport/wall.jpg",
        GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE, GL_REPEAT, GL_TRUE);

    pScene->addSkyboxImage(
        "../resources/textures/skybox/SunSet/SunSetLeft2048.png",
        "../resources/textures/skybox/SunSet/SunSetRight2048.png",
        "../resources/textures/skybox/SunSet/SunSetUp2048.png",
        "../resources/textures/skybox/SunSet/SunSetDown2048.png",
        "../resources/textures/skybox/SunSet/SunSetFront2048.png",
        "../resources/textures/skybox/SunSet/SunSetBack2048.png");

}

// Do integration
void MeshlessDeformation::integration(
    const float deltaTime)
{
    const std::vector<Math::Vector3>& v  = m_pPhysicsSys->velocityIntegration(deltaTime);

    std::vector<Mesh::Vertex>& vertexRef = m_pMesh->m_vertexBuffer;

    for (size_t i = 0; i < m_meshDataCount; ++i)
    {
        // v = v + delta * (f / m) + alpha / delta * (gi - xi')
        // v[i] += (alpha / deltaTime) * (g[i] - vertexRef[i].position);

        vertexRef[i].position += const_cast<Math::Vector3&>(v[i]) * deltaTime;
    }
}

void MeshlessDeformation::run()
{
    while (!glfwWindowShouldClose(m_context.GetWindow()))
    {
        GLfloat curTime = (GLfloat)glfwGetTime();
        GLfloat deltaTime = curTime - prevTime;
        prevTime = curTime;

        update(deltaTime);

        draw();

        glfwSwapBuffers(m_context.GetWindow());
    }

    glfwTerminate();
}

void MeshlessDeformation::update(
    const float deltaTime)
{
    glfwPollEvents();

    pScene->update(deltaTime);

    for (const Mesh::Vertex v : m_pMesh->m_vertexBuffer)
    {
        t += v.position;
    }
    t *= m_meshDataCountInv;

    Mat3 Apq;
    for (UINT i = 0; i < m_meshDataCount; ++i)
    {
        p[i] = m_pMesh->m_vertexBuffer[i].position - t;

        Apq += Math::Mat3(p[i].x * q[i].x, p[i].x * q[i].y, p[i].x * q[i].z,
                          p[i].y * q[i].x, p[i].y * q[i].y, p[i].y * q[i].z,
                          p[i].z * q[i].x, p[i].z * q[i].y, p[i].z * q[i].z);
    }

    A = Apq * Aqq;

    Mat3 Q = A.Transpose() * A; // Q = A^T * A

    // Symmetric matrix
    Mat3 S;


    Mat3 R = A * S.Inverse(); // R = A * S^-1

    integration(deltaTime);

    //m_pMesh->updateVertexData();
}

void MeshlessDeformation::draw()
{
    pScene->draw();
}
