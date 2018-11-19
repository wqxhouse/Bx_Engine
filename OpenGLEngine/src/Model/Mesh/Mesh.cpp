//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#include "Mesh.h"
#include "../../Memory/MemoryPool.h"

Mesh::Mesh(
    const std::string&                name,
    const std::string&                materialFile,
    const int                         counter[],
    const std::vector<Math::Vector3>& posBuf, 
    const std::vector<Math::Vector3>& normalBuf,
    const std::vector<Math::Vector2>& texCoords, 
    const std::vector<GLuint>&        indices)
    : m_name(name),
      m_materialName(materialFile),
      m_pMaterial(NULL),
      m_pMaterialMap(NULL),
      useGlobalMaterial(FALSE)
{
    initialize();
}

Mesh::Mesh(
    const std::string&                name,
    const std::string&                materialFile,
    const int                         counter[],
    const std::vector<Math::Vector3>& posBuf, 
    const std::vector<Math::Vector3>& normalBuf,
    const std::vector<Math::Vector2>& texCoords, 
    const std::vector<GLuint>&        indices, 
    const std::vector<Texture*>&      textures)
    : m_name(name),
      m_materialName(materialFile),
      m_pMaterial(NULL),
      useGlobalMaterial(FALSE)
{
    initialize();
}

Mesh::Mesh(
    const std::string&                name,
    const std::string&                materialFile,
    const int                         counter[],
    const std::vector<Math::Vector3>& posBuf,
    const std::vector<Math::Vector3>& normalBuf,
    const std::vector<Math::Vector2>& texCoords,
    const std::vector<GLuint>&        posIndices,
    const std::vector<GLuint>&        normalIndices,
    const std::vector<GLuint>&        texCoordIndices)
    : m_name(name),
      m_materialName(materialFile),
      m_pMaterial(NULL),
      useGlobalMaterial(FALSE)
{
    combineVertexData(counter, posBuf, normalBuf, texCoords,
                      posIndices, normalIndices, texCoordIndices);

    initialize();
}

Mesh::Mesh(
    const std::string&                name,
    const std::string&                materialFile,
    const int                         counter[],
    const std::vector<Math::Vector3>& posBuf,
    const std::vector<Math::Vector3>& normalBuf,
    const std::vector<Math::Vector2>& texCoords,
    const std::vector<GLuint>&        posIndices,
    const std::vector<GLuint>&        normalIndices,
    const std::vector<GLuint>&        texCoordIndices,
    const std::vector<Texture*>&      textures)
    : m_name(name),
      m_materialName(materialFile),
      m_pMaterial(NULL),
      useGlobalMaterial(FALSE)
{
    //TODO: Safely register the memory
    //Memory::MemoryPool::registerMemory<Mesh>(this);

    combineVertexData(counter, posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);
    //this->m_textureList = textures;

    initialize();
}

void Mesh::initialize()
{
    glGenVertexArrays(1, &m_vertexArrayObj);
    glGenBuffers(1, &m_vertexBufferObj);
    glGenBuffers(1, &m_indexBufferObj);

    glBindVertexArray(m_vertexArrayObj);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.size() * sizeof(Vertex),
                 m_vertexBuffer.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(GLuint),
                 m_indexBuffer.data(), GL_STATIC_DRAW);
    /*
        glVertexAttribPointer:
        1. index in vertex shader,
        2. counts of every element,
        3. elements type,
        4. if need to be normalized to 0-1,
        5. space between attribute sets,
        6. offset in every stride
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::position)));

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::normal)));

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::texCoords)));

    switch (m_polyMode)
    {
    case POLYMODE_WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case POLYMODE_FILL:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }

    // Unbind VAO
    glBindVertexArray(0);

    /*m_vertexBuffer.clear();
    m_indexBuffer.clear();*/

    /*glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObj);
    GLint bufSize;

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufSize);
    Vertex* data = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, bufSize, GL_MAP_READ_BIT);*/

    // m_textureList.resize(MESH_TEXTURE_SIZE);
}

void Mesh::drawMeshPos(
    const GLuint shaderProgram)
{
    glBindVertexArray(m_vertexArrayObj);

    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::draw(
    const GLuint shaderProgram)
{
    if (useGlobalMaterial == FALSE)
    {
        assert(m_pUniformBufferMgr != NULL && m_pMaterial != NULL);

        switch (m_pMaterial->GetMaterialType())
        {
        case MaterialType::PHONG:
        {
            SpecularMaterial* pMaterial =
                static_cast<SpecularMaterial*>(m_pMaterial);

            m_pUniformBufferMgr->
                updateUniformBufferData(
                    m_materialBufferIndex,
                    sizeof(pMaterial->m_specularMaterialData),
                    &(pMaterial->m_specularMaterialData));

            break;
        }
        case MaterialType::COOKTORRANCE:
        {
            CookTorranceMaterial* pPbrMaterial =
                static_cast<CookTorranceMaterial*>(m_pMaterial);

            m_pUniformBufferMgr->
                updateUniformBufferData(
                    m_materialBufferIndex,
                    pPbrMaterial->GetOpaqueCookTorranceMaterialDataSize(),
                    pPbrMaterial->GetCookTorranceMaterialData());

            break;
        }
        default:
            printf("Unsupport material!\n");
            assert(FALSE);
            break;
        }
    }

    if (m_pMaterialMap->m_materialMapStruct.diffuseMap != NULL)
    {
        m_pMaterialMap->m_materialMapStruct.diffuseMap->bindTexture(GL_TEXTURE0, shaderProgram, "diffuseMap");
    }

    glBindVertexArray(m_vertexArrayObj);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::updateVertexData()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObj);
    auto err = glGetError();
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexBuffer.size() * sizeof(Mesh::Vertex), m_vertexBuffer.data());
    err = glGetError();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void* Mesh::mapVertexBufferData()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObj);
    GLint bufSize;

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufSize);

    GLvoid* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    Vertex* data = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, bufSize, GL_MAP_READ_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return data;
}

void Mesh::setMaterial(
    Material* pMaterial)
{
    if (m_pMaterial != NULL &&
        pMaterial->GetMaterialType() != m_pMaterial->GetMaterialType())
    {
        SafeDelete(m_pMaterial);
        m_pMaterial = pMaterial;
    }
    else
    {
        m_pMaterial = pMaterial;
    }
}

void Mesh::AddTexture(
    const std::string&           textureFile,
    const MESH_TEXTURE_MAP_TYPES type)
{
    Texture2D* pTexture = new Texture2D(textureFile);

    m_pMaterialMap->m_materialMapTextures[type] = pTexture;
}

void Mesh::updateMaterial(
    UniformBufferMgr* pUniformBufferMgr,
    const GLuint      materialBufferIndex)
{
    m_pUniformBufferMgr   = pUniformBufferMgr;
    m_materialBufferIndex = materialBufferIndex;
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vertexBufferObj);
    glDeleteBuffers(1, &m_vertexArrayObj);
    glDeleteBuffers(1, &m_indexBufferObj);

    //Memory::MemoryPool::release<Mesh>(this);
}

void Mesh::combineVertexData(
    const int                         counter[],
    const std::vector<Math::Vector3>& posBuf,
    const std::vector<Math::Vector3>& normalBuf,
    const std::vector<Math::Vector2>& texCoords,
    const std::vector<GLuint>&        posIndices,
    const std::vector<GLuint>&        normalIndices,
    const std::vector<GLuint>&        texCoordIndices)
{
    std::map<Vertex, GLuint> vertexIndexKey;

    UINT indicesBufferIndex = 0;

    size_t normalBufferSize = normalBuf.size();
    size_t texCoordsSize    = texCoords.size();

    assert(normalBufferSize > 0 || texCoordsSize > 0);

    for (int i = 0; i < counter[3]; ++i)
    {
        UINT posIndex    = 0;
        UINT normalIndex = 0;
        UINT uvIndex     = 0;

        if (posIndices.size() != 0)
        {
            posIndex = posIndices[i];
        }
        else
        {
            printf("Can't load the mesh without vertexes.\n");
            assert(FALSE);
        }

        normalIndex = normalIndices.size()   == 0 ? -1 : normalIndices[i];
        uvIndex     = texCoordIndices.size() == 0 ? -1 : texCoordIndices[i];    

        Math::Vector3 pos      = posBuf[posIndex];
        Math::Vector3 normal   =
            (((normalIndex < 0) || (normalBufferSize == 0)) ?
            Math::Vector3() : normalBuf[normalIndex]);

        Math::Vector2 texCoord =
            (((uvIndex < 0) || (texCoordsSize == 0)) ?
                Math::Vector2() : texCoords[uvIndex]);

        Vertex vertex          = Vertex(pos, normal, texCoord);
        
        GLuint index = -1;
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
    const std::map<Vertex, GLuint>& map,
    const Vertex&                   vertex,
    GLuint*                         index)
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
