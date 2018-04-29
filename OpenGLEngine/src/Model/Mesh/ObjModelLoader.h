#pragma once

#include <vector>
#include <unordered_map>

#include "../Model.h"
#include "ModelLoaderBase.h"

class ObjModelLoader : ModelLoaderBase
{
public:
	ObjModelLoader();
	void LoadModel(const string& modelFile, const string& materialFile, OUT Model* modelPtr);
	~ObjModelLoader();

	vector<Math::Vector3> posBuffer;
	vector<Math::Vector3> normalBuffer;
	vector<Math::Vector2> texCoords;

	vector<GLuint> posIndices;
	vector<GLuint> normalIndices;
	vector<GLuint> texIndices;
private:
	void parseIndices(const string& metadata, int* counter);
	void parseIndices(const string& metadata1, const string& metadata2, const string& metadata3, int* counter);
	void parseIndices(const vector<string> metadata, int* counter);

	void counter(const string& modelFile);

	void loadMaterial(const string& materialFile);

	uint32_t vertexCount;
	uint32_t normalCount;
	uint32_t texCoordCount;
	uint32_t indicesCount;

	std::string tempMeshName;
	std::string tempMaterialName;

	std::unordered_map<string, SpecularMaterial*> m_materialMap;
};