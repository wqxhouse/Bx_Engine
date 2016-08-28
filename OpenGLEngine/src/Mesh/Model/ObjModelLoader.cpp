/*
	Author: Xiangshun Bei
	Date: 08/28/2016
	Description: This class used to load mesh information and material
*/
#include <fstream>

#include "ObjModelLoader.h"

ObjModelLoader::ObjModelLoader()
{
}

void ObjModelLoader::LoadModel(const string& modelFile)
{
	std::string modelFileLine;
	std::ifstream inputStream(modelFile, std::ios::out);

	if (inputStream.is_open())
	{
		while(std::getline(inputStream, modelFileLine))
		{
			vector<string> vecPtr;
			split(modelFileLine, ' ', &vecPtr);

			if (vecPtr.size() > 2 && vecPtr.size() < 5)
			{
				if (vecPtr[0] == "v")
				{
					vector<float> pos(3, 0);
					pos[0] = stof(vecPtr[1]);
					pos[1] = stof(vecPtr[2]);
					pos[2] = stof(vecPtr[3]);
					posBuffer.push_back(pos);
				}
				else if (vecPtr[0] == "vn")
				{
					vector<float> normal(3, 0);
					normal[0] = stof(vecPtr[1]);
					normal[1] = stof(vecPtr[2]);
					normal[2] = stof(vecPtr[3]);
					normalBuffer.push_back(normal);
				}
				else if (vecPtr[0] == "vt")
				{
					vector<float> tex(2, 0);
					tex[0] = stof(vecPtr[1]);
					tex[1] = stof(vecPtr[2]);
					texBuffer.push_back(tex);
				}
				else if (vecPtr[0] == "vp")
				{
					//TODO
				}
				else if (vecPtr[0] == "f")
				{

				}
				else
				{
					throw exception("Invalid obj model format\n");
				}
			}
		}
	}
}

void ObjModelLoader::LoadModel(const string& modelFile, const string& materialFile)
{
	LoadModel(modelFile);

}

ObjModelLoader::~ObjModelLoader()
{
}
