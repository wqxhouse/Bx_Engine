#include "Model.h"
#include "Mesh/ObjModelLoader.h"

Model::Model(const std::string & modelFile, const std::string& materialFile, Transform * modelTrans)
	:trans(modelTrans)
{
	ObjModelLoader objLoader;
	objLoader.LoadModel(modelFile, materialFile, this);
}

Model::~Model()
{
	for (Mesh* pMesh : m_pMeshList)
	{
		delete(pMesh);
	}
	m_pMeshList.clear();

	delete(trans);
}

void Model::draw()
{
    for (Mesh* pMesh : m_pMeshList)
    {
        pMesh->draw();
    }
    //m_pMeshList[0]->draw();
    glBindVertexArray(0);
}
