/*
	Author: Xiangshun Bei
	Date: 08/28/2016
	Description: This class used to load mesh information and material
*/
#include <fstream>

#include "ObjModelLoader.h"

ObjModelLoader::ObjModelLoader()
{}

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
					Math::Vector3 pos;
					pos.X = stof(vecPtr[1]);
					pos.Y = stof(vecPtr[2]);
					pos.Z = stof(vecPtr[3]);
					posBuffer.push_back(pos);
				}
				else if (vecPtr[0] == "vn")
				{
					Math::Vector3 normal;
					normal.X = stof(vecPtr[1]);
					normal.Y = stof(vecPtr[2]);
					normal.Z = stof(vecPtr[3]);
					normalBuffer.push_back(normal);
				}
				else if (vecPtr[0] == "vt")
				{
					Math::Vector2 tex;
					tex.X = stof(vecPtr[1]);
					tex.Y = stof(vecPtr[2]);
					texCoords.push_back(tex);
				}
				else if (vecPtr[0] == "vp")
				{
					//TODO
				}
				else if (vecPtr[0] == "f")
				{
					try 
					{
						parseIndices(vecPtr[1], vecPtr[2], vecPtr[3]);
					}
					catch (exception e)
					{
						cerr << "Can't parse the indices data from the obj file: " << e.what() << endl;
						throw;
					}
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

void ObjModelLoader::LoadModel(const string & modelFile, OUT Mesh* meshPtr)
{
	LoadModel(modelFile);
	meshPtr = new Mesh(posBuffer, normalBuffer, texCoords, posIndices, normalIndices, texIndices);

}

//TODO: Optimize the parse method, no need to check count each time
void ObjModelLoader::parseIndices(const string & metadata)
{
	vector<string> indexData;
	split(metadata, '/', &indexData);
	int indexDataSize = indexData.size();

	if (indexDataSize == 1)
	{
		posIndices.push_back(stof(indexData[0]));
	}
	else if (indexDataSize == 2)
	{
		posIndices.push_back(stof(indexData[0]));
		for (int i = 0; i < metadata.size(); ++i)
		{
			if (metadata[i] == '/' && (i < metadata.size() && metadata[i + 1] == '/'))
			{
				normalIndices.push_back(stof(indexData[1]));
			}
			else
			{
				texIndices.push_back(stof(indexData[1]));
			}
		}
	}
	else if (indexDataSize == 3)
	{
		posIndices.push_back(stoi(indexData[0]));
		normalIndices.push_back(stoi(indexData[1]));
		texIndices.push_back(stoi(indexData[2]));
	}
	else
	{
		throw exception("The count of element per index is wrong: Should be 1,2 or 3 not %d", indexDataSize);
	}
}

void ObjModelLoader::parseIndices(const string & metadata1, const string & metadata2, const string & metadata3)
{
	parseIndices(metadata1);
	parseIndices(metadata2);
	parseIndices(metadata3);
}

ObjModelLoader::~ObjModelLoader()
{
}
