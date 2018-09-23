#pragma once

#include <Context\OpenGLContext.h>
#include "PhysicsSystem.h"

using namespace Math;

class MeshlessDeformation
{
public:
    MeshlessDeformation();
    ~MeshlessDeformation();

    void init();

    void run();

    // Integration
    void update(
        const float deltaTime);

    // Draw the scene
    void draw();

private:
    void initScene();

    void integration(
        const float deltaTime);

    // Render Context
    Setting m_setting;
    OpenGLContext m_context;

    Scene* pScene;

    GLfloat prevTime  = 0.0f;

    PhysicsSystem* m_pPhysicsSys;

    // Deformation Mesh
    Mesh*                      m_pMesh;
    UINT                       m_meshDataCount;
    float                      m_meshDataCountInv;

    // Meshless Deformation
    Math::Vector3 t0;             // Original Geometry center
    std::vector<Math::Vector3> q; // Original Relative location to geometry center

    Mat3 A;                       // Matrix A
    Mat3 Aqq;                     // Sigma(mi * qi * qi^T)

    Math::Vector3 t;              // Real-time geometry center
    std::vector<Math::Vector3> p; // Real-time relative location to geometry center

    std::vector<Math::Vector3> g; // Shape matching location

    float alpha;                  // Determine how rigid the body it is
    float beta;                   // Replace R with beta * A + (1 - beta) * R
};
