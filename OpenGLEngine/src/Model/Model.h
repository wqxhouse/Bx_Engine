#pragma once

#include "Mesh/Mesh.h"
#include "../Math/Transform/Transform.h"

class Model
{
public:
	Model(const std::string & modelFile, const std::string& materialFile, Transform* modelTrans);
	~Model();

    void drawModelPos();
    void draw();

    void updateMaterial(
        UniformBufferMgr* pUniformBufferMgr,
        const GLuint      materialBufferIndex);

    void updateMaterial(
        UniformBufferMgr* pUniformBufferMgr,
        const GLuint      materialBufferIndex,
        const UINT        meshIndex);

	std::vector<Mesh*> m_pMeshList;

	Transform* trans;
};