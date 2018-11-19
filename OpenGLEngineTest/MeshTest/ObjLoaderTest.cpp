//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include <Model/Model.h>
#include <Model/Mesh/ObjModelLoader.h>

#include "ObjLoaderTest.h"

void RunObjLoaderTest()
{
	ObjModelLoader loader;
	Model *pModel = NULL;

	string objPath = "C:/Study_SSD/Graphics/OpenGLTemplate/Resources/model/Cube/cube.obj";
		//"C:/Study_SSD/Graphics/OpenGLTemplate/Resources/model/sphere.obj";

	//loader.LoadModel(objPath, pModel);

	delete pModel;
}