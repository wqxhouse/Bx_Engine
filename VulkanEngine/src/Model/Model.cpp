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

//void Model::drawModelPos(
//    const GLuint shaderProgram)
//{
//    m_pTrans->update();
//
//    for (Mesh* pMesh : m_pMeshList)
//    {
//        pMesh->drawMeshPos(shaderProgram);
//    }
//
//    glBindVertexArray(0);
//}
//
//void Model::draw(
//    const GLuint shaderProgram)
//{
//    m_pTrans->update();
//
//    for (Mesh* pMesh : m_pMeshList)
//    {
//        pMesh->draw(shaderProgram);
//    }
//
//    glBindVertexArray(0);
//}

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

//void Model::updateMaterial(
//    UniformBufferMgr* pUniformBufferMgr,
//    const GLuint      materialBufferIndex)
//{
//    for (Mesh* pMesh : m_pMeshList)
//    {
//        pMesh->updateMaterial(pUniformBufferMgr, materialBufferIndex);
//    }
//}

//void Model::updateMaterial(
//    UniformBufferMgr* pUniformBufferMgr,
//    const GLuint      materialBufferIndex,
//    const UINT        meshIndex) 
//{
//    m_pMeshList[meshIndex]->
//        updateMaterial(pUniformBufferMgr, materialBufferIndex);
//}

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
