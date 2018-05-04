#include "Mesh.h"
#include "../../Memory/MemoryPool.h"

Mesh::Mesh(
	const std::string& name,
	const std::string& materialFile,
	const int counter[],
	const std::vector<Math::Vector3>& posBuf, 
	const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords, 
	const std::vector<GLuint>& indices)
	: m_name(name), m_materialName(materialFile)
{
	initialize();
}

Mesh::Mesh(
	const std::string& name,
	const std::string& materialFile,
	const int counter[],
	const std::vector<Math::Vector3>& posBuf, 
	const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords, 
	const std::vector<GLuint>& indices, 
	const std::vector<Texture>& textures)
	: m_name(name), m_materialName(materialFile)
{
	initialize();
}

Mesh::Mesh(
	const std::string& name,
	const std::string& materialFile,
	const int counter[],
	const std::vector<Math::Vector3>& posBuf,
	const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords,
	const std::vector<GLuint>& posIndices,
	const std::vector<GLuint>& normalIndices,
	const std::vector<GLuint>& texCoordIndices)
	: m_name(name), m_materialName(materialFile)
{
	combineVertexData(counter, posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);

	initialize();
}

Mesh::Mesh(
	const std::string& name,
	const std::string& materialFile,
	const int counter[],
	const std::vector<Math::Vector3>& posBuf,
	const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords,
	const std::vector<GLuint>& posIndices,
	const std::vector<GLuint>& normalIndices,
	const std::vector<GLuint>& texCoordIndices,
	const std::vector<Texture>& textures)
	: m_name(name), m_materialName(materialFile)
{
	//TODO: Safely register the memory
	Memory::MemoryPool::registerMemory<Mesh>(this);

	combineVertexData(counter, posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);
	this->textures = textures;

	initialize();
}

void Mesh::initialize()
{
	glGenVertexArrays(1, &m_vertexArrayObj);
	glGenBuffers(1, &m_vertexBufferObj);
	glGenBuffers(1, &m_indexBufferObj);

	glBindVertexArray(m_vertexArrayObj);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObj);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(GLfloat),
		         vertexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(GLuint),
                 indexBuffer.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		                  sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::position)));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		                  sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::normal)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		                  sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::texCoords)));
	glEnableVertexAttribArray(2);

	// Unbind VBO/VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw()
{
    glBindVertexArray(m_vertexArrayObj);

    switch(m_polyMode)
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

    glDrawElements(GL_TRIANGLES, indexBuffer.size(), GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &m_vertexBufferObj);
	glDeleteBuffers(1, &m_vertexArrayObj);
	glDeleteBuffers(1, &m_indexBufferObj);

	//Memory::MemoryPool::release<Mesh>(this);
	if (m_pMaterial != nullptr)
	{
		switch (m_pMaterial->materialType)
		{
		case SPECULAR:
			delete (SpecularMaterial*)m_pMaterial;
			break;
		case ALBEDO:
			// TODO: Release PBR materials
			break;
		default:
			delete m_pMaterial;
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
	int vertexBufferSize = 0;

	for (int i = 0; i < counter[3]; ++i)
	{
		int posIndex = 0;// = posIndices[i];
		int normalIndex = 0;// = normalIndices[i];
		int uvIndex = 0;// = texCoordIndices[i];

		if (posIndices.size() != 0)
		{
			posIndex = posIndices[i];
		}
		else
		{
			throw std::exception("Can't load the mesh without vertexes.\n");
		}

		normalIndex = normalIndices.size() == 0 ? -1 : normalIndices[i];
		uvIndex = texCoordIndices.size() == 0 ? -1 : texCoordIndices[i];

		Math::Vector3 pos = posBuf[posIndex];
		Math::Vector3 normal = normalIndex < 0 ? Math::Vector3() : normalBuf[normalIndex];
		Math::Vector2 texCoord = uvIndex < 0 ? Math::Vector2() : texCoords[uvIndex];
		Vertex vertex = Vertex(pos, normal, texCoord);
		
		GLuint index = -1;
		if (!findSimilarVertex(vertexIndexKey, vertex, &index))
		{
			//int indexBase = indicesBufferIndex * 8;
			//vertexBuffer[indexBase] = pos.X;
			//vertexBuffer[indexBase + 1] = pos.Y;
			//vertexBuffer[indexBase + 2] = pos.Z;

			//vertexBuffer[indexBase + 3] = normal.X;
			//vertexBuffer[indexBase + 4] = normal.Y;
			//vertexBuffer[indexBase + 5] = normal.Z;

			//vertexBuffer[indexBase + 6] = texCoord.X;
			//vertexBuffer[indexBase + 7] = texCoord.Y;
			vertexBuffer.push_back(pos.X);
			vertexBuffer.push_back(pos.Y);
			vertexBuffer.push_back(pos.Z);
			vertexBuffer.push_back(normal.X);
			vertexBuffer.push_back(normal.Y);
			vertexBuffer.push_back(normal.Z);
			vertexBuffer.push_back(texCoord.X);
			vertexBuffer.push_back(texCoord.Y);

			indexBuffer.push_back(indicesBufferIndex);
			vertexIndexKey[vertex] = indicesBufferIndex;

			++indicesBufferIndex;
		}
		else
		{
			indexBuffer.push_back(index);
		}
		/*vertexBuffer[posIndex].setPositionData(posBuf[posIndex]);

		if (normalIndices.size() != 0)
		{
			vertexBuffer[posIndex].setNormalData(normalBuf[normalIndex]);
		}

		if (texCoords.size() != 0)
		{
			vertexBuffer[posIndex].setTexCoords(texCoords[uvIndex]);
		}
		
		indicesSet.insert(posBuf[posIndex]);*/
	}

	//this->indexBuffer = posIndices;
}

bool Mesh::findSimilarVertex(const std::map<Vertex, GLuint>& map, const Vertex & vertex, GLuint * index)
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

