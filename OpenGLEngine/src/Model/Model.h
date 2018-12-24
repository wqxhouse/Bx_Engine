//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <unordered_set>

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

    void drawModelPos(const GLuint shaderProgram);
    void draw(const GLuint shaderProgram);

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

    void AddMesh(Mesh* pMesh, Material* pMaterial, MaterialMap* pMaterialMap) 
    {
        if (pMaterial != NULL)
        {
            m_pMaterialSet.insert(pMaterial);
            pMesh->setMaterial(pMaterial);
        }

        if (pMaterialMap != NULL)
        {
            m_pMaterialMapSet.insert(pMaterialMap);
            pMesh->setMaterialMap(pMaterialMap);
        }

        m_pMeshList.push_back(pMesh);
    }

    inline MaterialType GetModelMaterialType() const
    {
        assert(m_pMeshList.size() > 0);

        // Assumption: the material type for all meshes in a model are all the same.
        return m_pMeshList[0]->GetMaterial()->GetMaterialType();
    }

    inline Mesh*  GetMesh(const UINT i) const { return m_pMeshList[i]; }
    inline Trans* GetTrans()            const { return m_pTrans;       }

    void UseLocalMaterial();
    void UseGlobalMaterial();

    inline void UseLocalMaterial (const UINT meshIndex) const { m_pMeshList[meshIndex]->UseLocalMaterial();  }
    inline void UseGlobalMaterial(const UINT meshIndex) const { m_pMeshList[meshIndex]->UseGlobalMaterial(); }

private:
	std::vector<Mesh*> m_pMeshList;

    std::unordered_set<Material*> m_pMaterialSet;
    std::unordered_set<MaterialMap*> m_pMaterialMapSet;

	Trans* m_pTrans;

};
