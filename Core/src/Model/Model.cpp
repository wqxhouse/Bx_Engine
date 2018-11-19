#include "Model.h"
#include "Mesh/ObjModelLoader.h"

namespace Object
{
    namespace Model
    {
        ModelObject::ModelObject(
            const std::string& modelFile,
            const std::string& materialFile,
            Trans*             modelTrans)
            : m_pTrans(modelTrans)
        {
            ObjModelLoader objLoader;
            objLoader.LoadModel(modelFile, materialFile, this);
        }

        ModelObject::~ModelObject()
        {
            m_pMeshList.clear();

            for (Material* pMaterial : m_pMaterialSet)
            {
                SafeDelete(pMaterial);
            }
            m_pMaterialSet.clear();

            for (MaterialMap* pMaterialMap : m_pMaterialMapSet)
            {
                SafeDelete(pMaterialMap);
            }
            m_pMaterialMapSet.clear();

            SafeDelete(m_pTrans);
        }

        void ModelObject::updateMaterialData(
            Material* pMaterial)
        {
            for (shared_ptr<Mesh> pMesh : m_pMeshList)
            {
                pMesh->setMaterial(pMaterial);
            }
        }

        void ModelObject::updateMaterialData(
            Material*  pMaterial,
            const UINT meshIndex)
        {
            m_pMeshList[meshIndex]->setMaterial(pMaterial);
        }

        void ModelObject::UseLocalMaterial()
        {
            for (shared_ptr<Mesh> pMesh : m_pMeshList)
            {
                pMesh->UseLocalMaterial();
            }
        }

        void ModelObject::UseGlobalMaterial()
        {
            for (shared_ptr<Mesh> pMesh : m_pMeshList)
            {
                pMesh->UseGlobalMaterial();
            }
        }
    }
}