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

    void updateMaterialData(
        Material* pMaterial);

    void updateMaterialData(
        Material*  pMaterial,
        const UINT meshIndex);

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

    void UseLocalMaterial();
    void UseGlobalMaterial();

    inline void UseLocalMaterial (const UINT meshIndex) const { m_pMeshList[meshIndex]->UseLocalMaterial();  }
    inline void UseGlobalMaterial(const UINT meshIndex) const { m_pMeshList[meshIndex]->UseGlobalMaterial(); }

	std::vector<Mesh*> m_pMeshList;

	Trans* m_pTrans;

};
