#include "Model.h"
#include "Mesh/ObjModelLoader.h"

Model::Model(
    const std::string& modelFile,
    const std::string& materialFile,
    Trans*             modelTrans)
	: m_pTrans(modelTrans)
{
	ObjModelLoader objLoader;
	objLoader.LoadModel(modelFile, materialFile, this);
}

Model::~Model()
{
	for (Mesh* pMesh : m_pMeshList)
	{
		SafeDelete(pMesh);
	}
	m_pMeshList.clear();

    SafeDelete(m_pTrans);
}

void Model::drawModelPos()
{
    m_pTrans->update();

    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->drawMeshPos();
    }

    glBindVertexArray(0);
}

void Model::draw()
{
    m_pTrans->update();

    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->draw();
    }

    glBindVertexArray(0);
}

void Model::updateMaterialData(
    Material* pMaterial)
{
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->setMaterial(pMaterial);
    }
}

void Model::updateMaterialData(
    Material*  pMaterial,
    const UINT meshIndex)
{
    m_pMeshList[meshIndex]->setMaterial(pMaterial);
}

void Model::updateMaterial(
    UniformBufferMgr* pUniformBufferMgr,
    const GLuint      materialBufferIndex)
{
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->updateMaterial(pUniformBufferMgr, materialBufferIndex);
    }
}

void Model::updateMaterial(
    UniformBufferMgr* pUniformBufferMgr,
    const GLuint      materialBufferIndex,
    const UINT        meshIndex) 
{
    m_pMeshList[meshIndex]->
        updateMaterial(pUniformBufferMgr, materialBufferIndex);
}

void Model::UseLocalMaterial()
{
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->UseLocalMaterial();
    }
}

void Model::UseGlobalMaterial()
{
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->UseGlobalMaterial();
    }
}
