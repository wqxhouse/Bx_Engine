#include "Model.h"
#include "Mesh\ObjModelLoader.h"

Model::Model(const std::string & modelFile, Transform * modelTrans)
	:trans(modelTrans)
{
	ObjModelLoader objLoader;
	objLoader.LoadModel(modelFile, this);
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