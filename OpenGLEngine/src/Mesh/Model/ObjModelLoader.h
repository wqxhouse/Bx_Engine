#pragma once

#include <vector>

#include "ModelLoaderBase.h"

class ObjModelLoader : ModelLoaderBase
{
public:
	ObjModelLoader();
	void LoadModel(const string& modelfile);
	void LoadModel(const string& modelFile, const string& materialFile);
	~ObjModelLoader();
private:
	void parseIndices(const string& metadata);
	void parseIndices(const string& metadata1, const string& metadata2, const string& metadata3);

	vector<Math::Vector3> posBuffer;
	vector<Math::Vector3> normalBuffer;
	vector<Math::Vector2> texBuffer;

	vector<float> posIndices;
	vector<float> normalIndices;
	vector<float> texIndices;
};