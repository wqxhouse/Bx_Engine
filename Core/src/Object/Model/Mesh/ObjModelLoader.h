//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <vector>
#include <unordered_map>

#include "../Model.h"
#include "ModelLoaderBase.h"

namespace Object
{
    namespace Model
    {
        class ObjModelLoader : ModelLoaderBase
        {
        public:
            ObjModelLoader();
            ~ObjModelLoader();

            void LoadModel(
                const string&    modelFile,
                const string&    materialFile,
                OUT ModelObject* modelPtr);

            vector<Math::Vector3> posBuffer;
            vector<Math::Vector3> normalBuffer;
            vector<Math::Vector2> texCoords;

            vector<UINT> posIndices;
            vector<UINT> normalIndices;
            vector<UINT> texIndices;
        private:
            void parseIndices(const string& str, int* counter);
            void parseIndices(const string& metadata1, const string& metadata2, const string& metadata3, int* counter);
            void parseIndices(const vector<string> metadata, int* counter);

            void counter(const string& modelFile);

            void loadMaterial(const string& materialFile);

            uint32_t vertexCount;
            uint32_t normalCount;
            uint32_t texCoordCount;
            uint32_t indicesCount;

            std::string tempMeshName;
            std::string tempMaterialName;

            std::unordered_map<string, PhongMaterial*> m_materialHashMap;
            std::unordered_map<string, MaterialMap*> m_materialTextureHashMap;
        };
    }
}