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
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->drawMeshPos();
    }

    glBindVertexArray(0);
}

void Model::draw()
{
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->draw();
    }

    glBindVertexArray(0);
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
