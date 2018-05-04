#pragma once

#include "Mesh/Mesh.h"
#include "../Math/Transform/Transform.h"

class Model
{
public:
	Model(const std::string & modelFile, const std::string& materialFile, Transform* modelTrans);
	~Model();

    void draw();

	std::vector<Mesh*> m_pMeshList;

	Transform* trans;
};