#pragma once

#include "../../Core/VulkanPCH.h"
#include "../../Math/Vector3.h"

using namespace std;

class ModelLoaderBase
{
public:
	ModelLoaderBase();
	void virtual LoadModel(const string &fileName);
	~ModelLoaderBase();
};

