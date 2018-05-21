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
    : m_name(name), m_materialName(materialFile)
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
    : m_name(name), m_materialName(materialFile)
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
    : m_name(name), m_materialName(materialFile)
{
    combineVertexData(counter, posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);

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
    : m_name(name), m_materialName(materialFile)
{
    //TODO: Safely register the memory
    //Memory::MemoryPool::registerMemory<Mesh>(this);

    combineVertexData(counter, posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);
    this->m_textureList = textures;

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
                 m_vertexBuffer.data(), GL_STATIC_DRAW);

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
    case WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case TRIANGLE:
        glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
        break;
    default:
        break;
    }

    // Unbind VBO/VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::drawMeshPos()
{
    glBindVertexArray(m_vertexArrayObj);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    //glDisableVertexAttribArray(1);
    //glDisableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::draw()
{
    if (m_pUniformBufferMgr != NULL && m_pMaterial != NULL)
    {
        SpecularMaterial* pMaterial =
            static_cast<SpecularMaterial*>(m_pMaterial);
        /*pMaterial->kd = Vector3(0.6f, 0.6f, 0.6f);
        pMaterial->ks = Vector3(0.4f, 0.4f, 0.4f);
        pMaterial->ns = 50.0f;*/

        m_pUniformBufferMgr->
            updateUniformBufferData(
                m_materialBufferIndex,
                sizeof(pMaterial->m_materialData),
                &(pMaterial->m_materialData));
    }
    else
    {
        assert("The material pointer is NULL!");
    }

    glBindVertexArray(m_vertexArrayObj);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    if (m_pMaterial != NULL)
    {
        switch (m_pMaterial->materialType)
        {
        case SPECULAR:
            SafeDelete(static_cast<SpecularMaterial*>(m_pMaterial));
            break;
        case ALBEDO:
            SafeDelete(static_cast<SpecularMaterial*>(m_pMaterial));
            break;
        default:
            SafeDelete(m_pMaterial);
            break;
        }
    }
}

void Mesh::combineVertexData(
    const int counter[],
    const std::vector<Math::Vector3>& posBuf,
    const std::vector<Math::Vector3>& normalBuf,
    const std::vector<Math::Vector2>& texCoords,
    const std::vector<GLuint>& posIndices,
    const std::vector<GLuint>& normalIndices,
    const std::vector<GLuint>& texCoordIndices)
{
    std::map<Vertex, GLuint> vertexIndexKey;

    int indicesBufferIndex = 0;
    int m_vertexBufferSize = 0;

    for (int i = 0; i < counter[3]; ++i)
    {
        int posIndex    = 0;
        int normalIndex = 0;
        int uvIndex     = 0;

        if (posIndices.size() != 0)
        {
            posIndex = posIndices[i];
        }
        else
        {
            assert("Can't load the mesh without vertexes.\n");
        }

        normalIndex = normalIndices.size()   == 0 ? -1 : normalIndices[i];
        uvIndex     = texCoordIndices.size() == 0 ? -1 : texCoordIndices[i];

        Math::Vector3 pos      = posBuf[posIndex];
        Math::Vector3 normal   = normalIndex < 0 ? Math::Vector3() : normalBuf[normalIndex];
        Math::Vector2 texCoord = uvIndex < 0 ? Math::Vector2() : texCoords[uvIndex];
        Vertex vertex          = Vertex(pos, normal, texCoord);
        
        GLuint index = -1;
        if (!findSimilarVertex(vertexIndexKey, vertex, &index))
        {
            //int indexBase = indicesBufferIndex * 8;
            //m_vertexBuffer[indexBase] = pos.X;
            //m_vertexBuffer[indexBase + 1] = pos.Y;
            //m_vertexBuffer[indexBase + 2] = pos.Z;

            //m_vertexBuffer[indexBase + 3] = normal.X;
            //m_vertexBuffer[indexBase + 4] = normal.Y;
            //m_vertexBuffer[indexBase + 5] = normal.Z;

            //m_vertexBuffer[indexBase + 6] = texCoord.X;
            //m_vertexBuffer[indexBase + 7] = texCoord.Y;
            /*m_vertexBuffer.push_back(pos.X);
            m_vertexBuffer.push_back(pos.Y);
            m_vertexBuffer.push_back(pos.Z);
            m_vertexBuffer.push_back(normal.X);
            m_vertexBuffer.push_back(normal.Y);
            m_vertexBuffer.push_back(normal.Z);
            m_vertexBuffer.push_back(texCoord.X);
            m_vertexBuffer.push_back(texCoord.Y);*/
            m_vertexBuffer.push_back(Vertex(pos, normal, texCoord));

            m_indexBuffer.push_back(indicesBufferIndex);
            vertexIndexKey[vertex] = indicesBufferIndex;

            ++indicesBufferIndex;
        }
        else
        {
            m_indexBuffer.push_back(index);
        }
        /*m_vertexBuffer[posIndex].setPositionData(posBuf[posIndex]);

        if (normalIndices.size() != 0)
        {
            m_vertexBuffer[posIndex].setNormalData(normalBuf[normalIndex]);
        }

        if (texCoords.size() != 0)
        {
            m_vertexBuffer[posIndex].setTexCoords(texCoords[uvIndex]);
        }
        
        indicesSet.insert(posBuf[posIndex]);*/
    }

    //this->m_indexBuffer = posIndices;
}

bool Mesh::findSimilarVertex(
    const std::map<Vertex, GLuint>& map,
    const Vertex& vertex,
    GLuint * index)
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
