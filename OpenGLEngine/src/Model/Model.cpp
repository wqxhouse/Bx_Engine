#include "Model.h"
#include "Mesh\ObjModelLoader.h"

Model::Model(const std::string & modelFile, Transform * modelTrans)
	:trans(modelTrans)
{
	ObjModelLoader objLoader;
	objLoader.LoadModel(modelFile, &mesh);
}

Model::Model(const std::string & modelFile, const std::string& materialFile, Transform* modelTrans)
	:trans(modelTrans)
{
	ObjModelLoader objLoader;
	objLoader.LoadModel(modelFile, materialFile, &mesh);
}

Model::~Model()
{
	delete(mesh);
	delete(trans);
}