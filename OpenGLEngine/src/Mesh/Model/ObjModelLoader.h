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
	
	std::vector<float> vertexBuffer;
private:
	std::vector<vector<float>> posBuffer;
	std::vector<vector<float>> normalBuffer;
	std::vector<vector<float>> texBuffer;
};