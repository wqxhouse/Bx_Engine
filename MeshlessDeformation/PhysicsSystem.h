//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Core/TypeDef.h>
#include <Math/Math.h>

class PhysicsSystem
{
public:
    PhysicsSystem(
        const UINT  verticesNum,
        const float m = 1.0f);
    ~PhysicsSystem();

    Math::Vector3 calAcceleration() const;

    std::vector<Math::Vector3> velocityIntegration(
        const float deltaTime);

    void addExternalForce(
        const Math::Vector3& f);
    void clearExternalForce();

private:
    const Math::Vector3 gravity;               // Gravity

    std::vector<Math::Vector3> externalForces; // External force list

    float invM; // 1 / mass

    std::vector<Math::Vector3> velocity;
};