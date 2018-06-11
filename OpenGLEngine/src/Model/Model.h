#pragma once

#include "Mesh/Mesh.h"
#include "../Math/Transform/Trans.h"

class Model
{
public:
	Model(
        const std::string& modelFile,
        const std::string& materialFile,
        Trans* modelTrans);

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

    inline MaterialType GetModelMaterialType() const
    {
        assert(m_pMeshList.size() > 0);

        // Assumption: the material type for all meshes in a model are all the same.
        return m_pMeshList[0]->m_pMaterial->GetMaterialType();
    }

	std::vector<Mesh*> m_pMeshList;

	Trans* m_pTrans;
};
