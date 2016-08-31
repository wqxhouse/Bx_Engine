#include "Mesh\Model\ObjModelLoader.h"

int main()
{
	ObjModelLoader loader;
	Mesh *mesh;

	loader.LoadModel("C:/Study_SSD/Graphics/OpenGLTemplate/Resources/model/sphere.obj", &mesh);

	delete mesh;
	return 0;
}