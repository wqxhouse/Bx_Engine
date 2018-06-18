/*
    Author: Xiangshun Bei
    Date: 08/28/2016
    Description: This class used to load mesh information and material
*/
#include <fstream>

#include "ObjModelLoader.h"

ObjModelLoader::ObjModelLoader()
    :vertexCount(0), normalCount(0), texCoordCount(0), indicesCount(0)
{}

void ObjModelLoader::LoadModel(
    const string& modelFile,
    const string& materialFile,
    Model* modelPtr)
{
    loadMaterial(materialFile);
    counter(modelFile);

    ifstream inputStream(modelFile, std::ios::out);

    if (inputStream.is_open())
    {
        posBuffer.resize(vertexCount);
        normalBuffer.resize(normalCount);
        texCoords.resize(texCoordCount);

        posIndices.resize(indicesCount);
        normalIndices.resize(indicesCount);
        texIndices.resize(indicesCount);

        std::string modelFileLine;
        int counter[4] = { 1, 1, 1, 0 };//pos, normal, uv, index

        while (std::getline(inputStream, modelFileLine))
        {
            vector<string> vecPtr = split(modelFileLine, ' ');

            if (vecPtr.size() > 2)
            {
                if (vecPtr[0] == "v")
                {
                    posBuffer[counter[0]].setData(stof(vecPtr[1]), stof(vecPtr[2]), stof(vecPtr[3]));
                    counter[0] += 1;
                }
                else if (vecPtr[0] == "vn")
                {
                    normalBuffer[counter[1]].setData(stof(vecPtr[1]), stof(vecPtr[2]), stof(vecPtr[3]));
                    counter[1] += 1;
                }
                else if (vecPtr[0] == "vt")
                {
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
                        //parseIndices(vecPtr[1], vecPtr[2], vecPtr[3], &(counter[3]));
                        size_t size= vecPtr.size();
                        for (size_t i = 1; i < size; ++i)
                        {
                            parseIndices(vecPtr[i], &(counter[3]));
                        }
                    }
                    catch (exception e)
                    {
                        cerr << "Can't parse the indices data from the obj file: " << e.what() << endl;
                        throw;
                    }
                }
                else if (vecPtr[1] == "Object")
                {
                    if (tempMeshName == "")
                    {
                        tempMeshName = vecPtr[2];
                    }
                    else
                    {
                        Mesh* meshPtr = new Mesh(tempMeshName, tempMaterialName, counter,
                                                 posBuffer, normalBuffer, texCoords,
                                                 posIndices, normalIndices, texIndices);

                        if (m_materialMap.find(tempMaterialName) != m_materialMap.end())
                        {
                            meshPtr->m_pMaterial = m_materialMap[tempMaterialName];
                        }

                        modelPtr->m_pMeshList.push_back(meshPtr);

                        counter[3] = 0;

                        tempMeshName = vecPtr[2];
                    }
                }
                else
                {
                    //throw exception("Invalid obj model format\n");
                }
            }
            else if (vecPtr.size() == 2)
            {
                if (vecPtr[0] == "usemtl")
                {
                    tempMaterialName = vecPtr[1];
                }
            }
        }
        
        if (counter[3] > 0)
        {
            Mesh* meshPtr = new Mesh(tempMeshName, tempMaterialName, counter,
                posBuffer, normalBuffer, texCoords,
                posIndices, normalIndices, texIndices);

            if (m_materialMap.find(tempMaterialName) != m_materialMap.end())
            {
                meshPtr->m_pMaterial = m_materialMap[tempMaterialName];
            }

            modelPtr->m_pMeshList.push_back(meshPtr);
        }

        inputStream.close();
    }
}

//TODO: Optimize the parse method, no need to check count each time
void ObjModelLoader::parseIndices(
    const string& str,
    int* counter)
{
    //TODO: parse poly surface indices
    vector<string> indexData = split(str, '/');
    int indexDataSize = indexData.size();

    if (indexDataSize == 1)
    {
        posIndices[*counter] = stoi(indexData[0]);

        *counter += 1;
    }
    else if (indexDataSize == 2)
    {
        int slashSize = std::count(str.begin(), str.end(), '/');

        posIndices[*counter] = stoi(indexData[0]);

        if (slashSize == 1)
        {
            texIndices[*counter] = stoi(indexData[1]);
        }
        else
        {
            normalIndices[*counter] = stoi(indexData[1]);
        }

        *counter += 1;
    }
    else if (indexDataSize == 3)
    {
        posIndices[*counter] = stoi(indexData[0]);

        if (indexData[1] != "")
        {
            texIndices[*counter] = stoi(indexData[1]);
        }

        if (indexData[2] != "")
        {
            normalIndices[*counter] = stoi(indexData[2]);
        }

        *counter += 1;
    }
    else
    {
        //throw exception("The count of element per index is wrong: Should be 1,2 or 3 not %d", indexDataSize);
    }
}

void ObjModelLoader::parseIndices(
    const string & metadata1,
    const string & metadata2,
    const string & metadata3,
    int* counter)
{
    parseIndices(metadata1, counter);
    parseIndices(metadata2, counter);
    parseIndices(metadata3, counter);
}

void ObjModelLoader::parseIndices(
    const vector<string> metadata,
    int* counter)
{
    for(string s : metadata)
    {
        parseIndices(s, counter);
    }
}

void ObjModelLoader::counter(const string& modelFile)
{
    ifstream inputStream(modelFile, std::ios::out);
    string line;

    while (std::getline(inputStream, line))
    {
        stringReplace(&line, '\t');

        vector<string> vecPtr = split(line, ' ');
        if (vecPtr.size() > 2)
        {
            if (vecPtr[0] == "v") 
            { 
                if (vertexCount == 0)
                {
                    vertexCount = 1;
                }
                vertexCount += 1; 
            }
            else if (vecPtr[0] == "vn")
            {
                if (normalCount == 0)
                {
                    normalCount = 1;
                }
                normalCount += 1;
            }
            else if (vecPtr[0] == "vt") 
            { 
                if (texCoordCount == 0)
                {
                    texCoordCount = 1;
                }
                texCoordCount += 1; 
            }
            else if (vecPtr[0] == "f") 
            { 
                for (size_t i = 1; i < vecPtr.size(); ++i)
                {
                    size_t findCount = vecPtr[i].find('/');
                    if (findCount != string::npos)
                    {
                        indicesCount += 1;
                    }
                }
            }
        }
    }
    inputStream.close();
}

void ObjModelLoader::loadMaterial(const string & materialFile)
{
    ifstream inputStream(materialFile, std::ios::out);

    if (inputStream.is_open())
    {
        string line;

        while (std::getline(inputStream, line))
        {
            stringReplace(&line, '\t');

            vector<string> materialStrs = split(line, ' ');
            size_t strLength = materialStrs.size();

            if (strLength > 2)
            {
                if (materialStrs[0] == "Ka")
                {
                    m_materialMap[tempMaterialName]->
                        ka.setData(stof(materialStrs[1]), 
                                   stof(materialStrs[2]), 
                                   stof(materialStrs[3]));
                }
                else if (materialStrs[0] == "Kd")
                {
                    m_materialMap[tempMaterialName]->
                        kd.setData(stof(materialStrs[1]),
                                   stof(materialStrs[2]),
                                   stof(materialStrs[3]));
                }
                else if (materialStrs[0] == "Ks")
                {
                    m_materialMap[tempMaterialName]->
                        ks.setData(stof(materialStrs[1]),
                                   stof(materialStrs[2]),
                                   stof(materialStrs[3]));
                }
            }
            else if (strLength == 2)
            {
                if (materialStrs[0] == "newmtl")
                {
                    tempMaterialName = materialStrs[1];
                    m_materialMap[materialStrs[1]] = new SpecularMaterial(tempMaterialName);
                }
                else if (materialStrs[0] == "Ns") // Specular exponent
                {
                    m_materialMap[tempMaterialName]->ns = stof(materialStrs[1]);
                }
                else if (materialStrs[0] == "Ni")
                {
                    // TODO
                }
                else if (materialStrs[0] == "illum")
                {
                    // TODO
                }
            }
        }

        inputStream.close();
    }
}

ObjModelLoader::~ObjModelLoader()
{
}
