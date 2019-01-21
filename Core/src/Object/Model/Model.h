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
#include "../ObjectBase.h"

namespace Object
{
    namespace Model
    {
        class ModelObject : public ObjectBase
        {
        public:
            ModelObject(
                const UINT         objectId,
                const std::string& modelFile,
                const std::string& materialFile,
                Trans*             modelTrans);

            ~ModelObject();

            /*void drawModelPos(const UINT shaderProgram);
            void draw(const UINT shaderProgram);*/

            void updateMaterialData(
                Material* pMaterial);

            void updateMaterialData(
                Material*  pMaterial,
                const UINT meshIndex);

            /*void updateMaterial(
                UniformBufferMgr* pUniformBufferMgr,
                const UINT        materialBufferIndex);

            void updateMaterial(
                UniformBufferMgr* pUniformBufferMgr,
                const UINT        materialBufferIndex,
                const UINT        meshIndex);*/

            INLINE void AddMesh(
                Mesh*        pMesh,
                Material*    pMaterial,
                MaterialMap* pMaterialMap)
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

                m_pMeshList.push_back(std::shared_ptr<Mesh>(std::move(pMesh)));
            }

            INLINE MaterialType GetModelMaterialType() const
            {
                assert(m_pMeshList.size() > 0);

                // Assumption: the material type for all meshes in a model are all the same.
                return m_pMeshList[0]->GetMaterial()->GetMaterialType();
            }

            INLINE const UINT GetMeshNum() const
            {
                return static_cast<UINT>(m_pMeshList.size());
            }

            INLINE std::shared_ptr<Object::Model::Mesh> GetMesh(
                const UINT i) const 
            {
                return m_pMeshList[i];
            }

            void UseLocalMaterial();
            void UseGlobalMaterial();

            INLINE void UseLocalMaterial(const UINT meshIndex) const { m_pMeshList[meshIndex]->UseLocalMaterial(); }
            INLINE void UseGlobalMaterial(const UINT meshIndex) const { m_pMeshList[meshIndex]->UseGlobalMaterial(); }

        private:
            std::vector<std::shared_ptr<Mesh>> m_pMeshList;

            std::unordered_set<Material*>      m_pMaterialSet;
            std::unordered_set<MaterialMap*>   m_pMaterialMapSet;
        };
    }
}