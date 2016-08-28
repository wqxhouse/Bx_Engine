#pragma once

#include "../../Core/OpenGLPCH.h"
#include "../../Math/Vector3.h"

using namespace std;

class ModelLoaderBase
{
public:
	enum ModelFormat
	{
		OBJ, FBX
	};

	ModelLoaderBase();
	void virtual LoadModel(const string &fileName);
	~ModelLoaderBase();

	vector<float> vertexBuffer;
	vector<float> indexBuffer;
protected:
	void split(const string &str, char delim, vector<string>* splitedStrings);
};

