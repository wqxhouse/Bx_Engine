#pragma once

#include "Mesh/Mesh.h"
#include "../Math/Transform/Transform.h"

class Model
{
public:
	Model(const std::string & modelFile, Transform* modelTrans);
	~Model();

	Mesh* mesh;
	Transform* trans;
};