#pragma once

#include <vector>

#include "../Model.h"
#include "ModelLoaderBase.h"

class ObjModelLoader : ModelLoaderBase
{
public:
	ObjModelLoader();
	void LoadModel(const string& modelFile, OUT Model* modelPtr);
	~ObjModelLoader();

	vector<Math::Vector3> posBuffer;
	vector<Math::Vector3> normalBuffer;
	vector<Math::Vector2> texCoords;

	vector<GLuint> posIndices;
	vector<GLuint> normalIndices;
	vector<GLuint> texIndices;
private:
	void LoadModel(const string& modelFile);
	
	void parseIndices(const string& metadata, int* counter);
	void parseIndices(const string& metadata1, const string& metadata2, const string& metadata3, int* counter);
	void parseIndices(const vector<string> metadata, int* counter);

	void counter(const string& modelFile);
	uint32_t vertexCount;
	uint32_t normalCount;
	uint32_t texCoordCount;
	uint32_t indicesCount;

	std::string tempMeshName;
	std::string tempMaterialName;
};