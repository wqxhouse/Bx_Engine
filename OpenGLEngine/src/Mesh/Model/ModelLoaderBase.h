#pragma once

#include "../../Core/OpenGLPCH.h"

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
protected:
	void split(const string &str, char delim, vector<string>* splitedStrings);
};

