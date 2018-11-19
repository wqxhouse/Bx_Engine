//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "../../Core/OpenGLPCH.h"
#include "../../Math/Vector3.h"

using namespace std;

class ModelLoaderBase
{
public:
	ModelLoaderBase();
	void virtual LoadModel(const string &fileName);
	~ModelLoaderBase();
};

