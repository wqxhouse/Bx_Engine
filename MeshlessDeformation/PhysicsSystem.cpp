#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(
    const UINT verticesNum,
    const float m)
    : gravity(0.0f, -0.0981f, 0.0f)
{
    velocity.resize(verticesNum);

    invM = 1.0f / m;
}

PhysicsSystem::~PhysicsSystem()
{
}

std::vector<Math::Vector3> PhysicsSystem::velocityIntegration(
    const float deltaTime)
{
    for (Math::Vector3& v : velocity)
    {
        v += calAcceleration() * deltaTime; // vt = v0 + at
    }

    return velocity;
}

Math::Vector3 PhysicsSystem::calAcceleration() const
{
    Math::Vector3 a;

    Math::Vector3 force = gravity;

    for (const Math::Vector3& v : externalForces)
    {
        force += v;
    }

    a = force * invM; // a = f / m

    return a;
}

void PhysicsSystem::addExternalForce(
    const Math::Vector3& f)
{
    externalForces.push_back(f);
}

void PhysicsSystem::clearExternalForce()
{
    externalForces.clear();
}
