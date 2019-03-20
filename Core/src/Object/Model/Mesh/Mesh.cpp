//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Mesh.h"

namespace Object
{
    namespace Model
    {
        Mesh::Mesh(
            const std::string&                name,
            const std::string&                materialFile,
            const int                         counter[],
            const std::vector<Math::Vector3>& posBuf,
            const std::vector<Math::Vector3>& normalBuf,
            const std::vector<Math::Vector2>& texCoords,
            const std::vector<UINT>&          indices)
            : m_name(name),
            m_materialName(materialFile),
            useGlobalMaterial(FALSE)
        {
            initialize();
        }

        //Mesh::Mesh(
        //    const std::string&                name,
        //    const std::string&                materialFile,
        //    const int                         counter[],
        //    const std::vector<Math::Vector3>& posBuf,
        //    const std::vector<Math::Vector3>& normalBuf,
        //    const std::vector<Math::Vector2>& texCoords, 
        //    const std::vector<UINT>&          indices, 
        //    const std::vector<Texture*>&      textures)
        //    : m_name(name),
        //      m_materialName(materialFile),
        //      m_pMaterial(NULL),
        //      useGlobalMaterial(FALSE)
        //{
        //    initialize();
        //}

        Mesh::Mesh(
            const std::string&                name,
            const std::string&                materialFile,
            const int                         counter[],
            const std::vector<Math::Vector3>& posBuf,
            const std::vector<Math::Vector3>& normalBuf,
            const std::vector<Math::Vector2>& texCoords,
            const std::vector<UINT>&          posIndices,
            const std::vector<UINT>&          normalIndices,
            const std::vector<UINT>&          texCoordIndices)
            : m_name(name),
            m_materialName(materialFile),
            useGlobalMaterial(FALSE)
        {
            combineVertexData(counter, posBuf, normalBuf, texCoords,
                posIndices, normalIndices, texCoordIndices);

            initialize();
        }

        //Mesh::Mesh(
        //    const std::string&                name,
        //    const std::string&                materialFile,
        //    const int                         counter[],
        //    const std::vector<Math::Vector3>& posBuf,
        //    const std::vector<Math::Vector3>& normalBuf,
        //    const std::vector<Math::Vector2>& texCoords,
        //    const std::vector<UINT>&        posIndices,
        //    const std::vector<UINT>&        normalIndices,
        //    const std::vector<UINT>&        texCoordIndices,
        //    const std::vector<Texture*>&      textures)
        //    : m_name(name),
        //      m_materialName(materialFile),
        //      m_pMaterial(NULL),
        //      useGlobalMaterial(FALSE)
        //{
        //    //TODO: Safely register the memory
        //    //Memory::MemoryPool::registerMemory<Mesh>(this);
        //
        //    combineVertexData(counter, posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);
        //    //this->m_textureList = textures;
        //
        //    initialize();
        //}

        void Mesh::initialize()
        {
        }

        void Mesh::setMaterial(
            Material* pMaterial)
        {
            if (m_pMaterial != NULL &&
                pMaterial->GetMaterialType() != m_pMaterial->GetMaterialType())
            {
                m_pMaterial.release();
                m_pMaterial = std::unique_ptr<Material>(pMaterial);
            }
            else
            {
                m_pMaterial = std::unique_ptr<Material>(pMaterial);
            }
        }

        //void Mesh::AddTexture(
        //    const std::string&           textureFile,
        //    const MESH_TEXTURE_MAP_TYPES type)
        //{
        //    Texture2D* pTexture = new Texture2D(textureFile);
        //
        //    m_pMaterialMap->m_materialMapTextures[type] = pTexture;
        //}

        //void Mesh::updateMaterial(
        //    UniformBufferMgr* pUniformBufferMgr,
        //    const UINT      materialBufferIndex)
        //{
        //    m_pUniformBufferMgr   = pUniformBufferMgr;
        //    m_materialBufferIndex = materialBufferIndex;
        //}

        Mesh::~Mesh()
        {
        }

        void Mesh::combineVertexData(
            const int                         counter[],
            const std::vector<Math::Vector3>& posBuf,
            const std::vector<Math::Vector3>& normalBuf,
            const std::vector<Math::Vector2>& texCoords,
            const std::vector<UINT>&          posIndices,
            const std::vector<UINT>&          normalIndices,
            const std::vector<UINT>&          texCoordIndices)
        {
            std::map<Vertex, UINT> vertexIndexKey;

            UINT indicesBufferIndex = 0;

            size_t normalBufferSize = normalBuf.size();
            size_t texCoordsSize = texCoords.size();

            assert(normalBufferSize > 0 || texCoordsSize > 0);

            for (int i = 0; i < counter[3]; ++i)
            {
                UINT posIndex = 0;
                UINT normalIndex = 0;
                UINT uvIndex = 0;

                if (posIndices.size() != 0)
                {
                    posIndex = posIndices[i];
                }
                else
                {
                    printf("Can't load the mesh without vertexes.\n");
                    assert(FALSE);
                }

                normalIndex = normalIndices.size() == 0 ? -1 : normalIndices[i];
                uvIndex = texCoordIndices.size() == 0 ? -1 : texCoordIndices[i];

                Math::Vector3 pos = posBuf[posIndex];
                Math::Vector3 normal =
                    (((normalIndex < 0) || (normalBufferSize == 0)) ?
                        Math::Vector3() : normalBuf[normalIndex]);

                Math::Vector2 texCoord =
                    (((uvIndex < 0) || (texCoordsSize == 0)) ?
                        Math::Vector2() : texCoords[uvIndex]);

                Vertex vertex = Vertex(pos, normal, texCoord);

                UINT index = -1;
                if (!findSimilarVertex(vertexIndexKey, vertex, &index))
                {
                    m_vertexBuffer.push_back(Vertex(pos, normal, texCoord));

                    m_indexBuffer.push_back(indicesBufferIndex);
                    vertexIndexKey[vertex] = indicesBufferIndex;

                    ++indicesBufferIndex;
                }
                else
                {
                    m_indexBuffer.push_back(index);
                }
            }
        }

        bool Mesh::findSimilarVertex(
            const std::map<Vertex, UINT>& map,
            const Vertex&                 vertex,
            UINT*                         index)
        {
            auto it = map.find(vertex);
            if (it == map.end())
            {
                return false;
            }
            else
            {
                *index = it->second;
                return true;
            }
        }
    }
}