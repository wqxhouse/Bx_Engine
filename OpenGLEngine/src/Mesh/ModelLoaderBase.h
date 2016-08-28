#pragma once
#include <string>

class ModelLoaderBase
{
public:
	enum ModelFormat
	{
		OBJ, FBX
	};

	ModelLoaderBase();
	void LoadModel(std::string fileName, ModelFormat = OBJ);
	~ModelLoaderBase();
};

