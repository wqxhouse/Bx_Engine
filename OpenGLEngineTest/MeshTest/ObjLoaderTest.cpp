#include "Mesh\Model\ObjModelLoader.h"

int main()
{
	ObjModelLoader loader;
	Mesh *mesh;

	string objPath = "C:/Study_SSD/Graphics/OpenGLTemplate/Resources/model/Cube/cube.obj";
		//"C:/Study_SSD/Graphics/OpenGLTemplate/Resources/model/sphere.obj";

	loader.LoadModel(objPath, &mesh);

	delete mesh;
	return 0;
}