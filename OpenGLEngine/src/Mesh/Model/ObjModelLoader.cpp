/*
	Author: Xiangshun Bei
	Date: 08/28/2016
	Description: This class used to load mesh information and material
*/
#include <fstream>

#include "ObjModelLoader.h"

ObjModelLoader::ObjModelLoader()
	:vertexCount(1), texCoordCount(1), indicesCount(0)
{}

void ObjModelLoader::LoadModel(const string& modelFile)
{
	ifstream inputStream(modelFile, std::ios::out);

	if (inputStream.is_open())
	{
		posBuffer.assign(vertexCount, Math::Vector3());
		normalBuffer.assign(vertexCount, Math::Vector3());
		texCoords.assign(texCoordCount, Math::Vector2());

		posIndices.assign(indicesCount, 0);
		normalIndices.assign(indicesCount, 0);
		texIndices.assign(indicesCount, 0);

		std::string modelFileLine;
		int counter[4] = { 0,0,0,0 };//pos, normal, uv, index

		while (std::getline(inputStream, modelFileLine))
		{
			vector<string> vecPtr;
			split(modelFileLine, ' ', &vecPtr);

			if (vecPtr.size() > 2 && vecPtr.size() < 5)
			{
				if (vecPtr[0] == "v")
				{
					//pos.X = stof(vecPtr[1]);
					//pos.Y = stof(vecPtr[2]);
					//pos.Z = stof(vecPtr[3]);
					//posBuffer.push_back(pos);
					posBuffer[counter[0]].setData(stof(vecPtr[1]), stof(vecPtr[2]), stof(vecPtr[3]));
					counter[0] += 1;
				}
				else if (vecPtr[0] == "vn")
				{
					/*Math::Vector3 normal;
					normal.X = stof(vecPtr[1]);
					normal.Y = stof(vecPtr[2]);
					normal.Z = stof(vecPtr[3]);
					normalBuffer.push_back(normal);*/
					normalBuffer[counter[1]].setData(stof(vecPtr[1]), stof(vecPtr[2]), stof(vecPtr[3]));
					counter[1] += 1;
				}
				else if (vecPtr[0] == "vt")
				{
					/*Math::Vector2 tex;
					tex.X = stof(vecPtr[1]);
					tex.Y = stof(vecPtr[2]);
					texCoords.push_back(tex);*/
					texCoords[counter[2]].setData(stof(vecPtr[1]), stof(vecPtr[2]));
					counter[2] += 1;
				}
				else if (vecPtr[0] == "vp")
				{
					//TODO
				}
				else if (vecPtr[0] == "f")
				{
					try
					{
						parseIndices(vecPtr[1], vecPtr[2], vecPtr[3], &(counter[3]));
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

		inputStream.close();
	}
}

void ObjModelLoader::LoadModel(const string& modelFile, const string& materialFile, OUT Mesh** meshPtr)
{
	counter(modelFile);
	LoadModel(modelFile);
	*meshPtr = new Mesh(posBuffer, normalBuffer, texCoords, posIndices, normalIndices, texIndices);
}

void ObjModelLoader::LoadModel(const string & modelFile, OUT Mesh** meshPtr)
{
	counter(modelFile);
	LoadModel(modelFile);
	*meshPtr = new Mesh(posBuffer, normalBuffer, texCoords, posIndices, normalIndices, texIndices);
}

//TODO: Optimize the parse method, no need to check count each time
void ObjModelLoader::parseIndices(const string & metadata, int* counter)
{
	vector<string> indexData;
	split(metadata, '/', &indexData);
	int indexDataSize = indexData.size();

	if (indexDataSize == 1)
	{
		//posIndices.push_back(stoi(indexData[0]));
		posIndices[*counter] = stoi(indexData[0]);
	}
	else if (indexDataSize == 2)
	{
		posIndices.push_back(stoi(indexData[0]));
		for (int i = 0; i < metadata.size(); ++i)
		{
			if (metadata[i] == '/' && (i < metadata.size() && metadata[i + 1] == '/'))
			{
				//normalIndices.push_back(stoi(indexData[1]));
				normalIndices[*counter] = stoi(indexData[1]);
			}
			else
			{
				//texIndices.push_back(stoi(indexData[1]));
				texIndices[*counter] = stoi(indexData[1]);
			}
		}
	}
	else if (indexDataSize == 3)
	{
		/*posIndices.push_back(stoi(indexData[0]));
		texIndices.push_back(stoi(indexData[1]));
		normalIndices.push_back(stoi(indexData[2]));*/
		posIndices[*counter] = stoi(indexData[0]);
		texIndices[*counter] = stoi(indexData[1]);
		normalIndices[*counter] = stoi(indexData[2]);
	}
	else
	{
		throw exception("The count of element per index is wrong: Should be 1,2 or 3 not %d", indexDataSize);
	}

	*counter += 1;
}

void ObjModelLoader::parseIndices(const string & metadata1, const string & metadata2, const string & metadata3, int* counter)
{
	parseIndices(metadata1, counter);
	parseIndices(metadata2, counter);
	parseIndices(metadata3, counter);
}

void ObjModelLoader::counter(const string& modelFile)
{
	ifstream inputStream(modelFile, std::ios::out);
	string line;

	while (std::getline(inputStream, line))
	{
		vector<string> vecPtr;
		split(line, ' ', &vecPtr);

		if (vecPtr[0] == "v") { vertexCount += 1; }
		else if (vecPtr[0] == "vt") { texCoordCount += 1; }
		else if (vecPtr[0] == "f") { indicesCount += 3; }
	}
	inputStream.close();
}

ObjModelLoader::~ObjModelLoader()
{
}
