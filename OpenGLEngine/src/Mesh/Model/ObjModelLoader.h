#pragma once

#include <vector>

#include "Mesh.h"
#include "ModelLoaderBase.h"

class ObjModelLoader : ModelLoaderBase
{
public:
	ObjModelLoader();
	void LoadModel(const string& modelFile, OUT Mesh* meshPtr);
	~ObjModelLoader();
private:
	void LoadModel(const string& modelFile);
	void LoadModel(const string& modelFile, const string& materialFile);
	
	void parseIndices(const string& metadata);
	void parseIndices(const string& metadata1, const string& metadata2, const string& metadata3);

	vector<Math::Vector3> posBuffer;
	vector<Math::Vector3> normalBuffer;
	vector<Math::Vector2> texCoords;

	vector<GLuint> posIndices;
	vector<GLuint> normalIndices;
	vector<GLuint> texIndices;
};