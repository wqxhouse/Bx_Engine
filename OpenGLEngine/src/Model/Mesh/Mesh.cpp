#include "Mesh.h"
#include "../../Memory/MemoryPool.h"

Mesh::Mesh(const std::vector<Math::Vector3>& posBuf, const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords, const std::vector<GLuint>& indices)
{
}

Mesh::Mesh(const std::vector<Math::Vector3>& posBuf, const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords, const std::vector<GLuint>& indices, const std::vector<Texture>& textures)
{
}

Mesh::Mesh(
	const std::vector<Math::Vector3>& posBuf,
	const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords,
	const std::vector<GLuint>& posIndices,
	const std::vector<GLuint>& normalIndices,
	const std::vector<GLuint>& texCoordIndices)
{
	combineVertexData(posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);
}

Mesh::Mesh(
	const std::vector<Math::Vector3>& posBuf,
	const std::vector<Math::Vector3>& normalBuf,
	const std::vector<Math::Vector2>& texCoords,
	const std::vector<GLuint>& posIndices,
	const std::vector<GLuint>& normalIndices,
	const std::vector<GLuint>& texCoordIndices,
	const std::vector<Texture>& textures)
{
	//TODO: Safely register the memory
	Memory::MemoryPool::registerMemory<Mesh>(this);

	combineVertexData(posBuf, normalBuf, texCoords, posIndices, normalIndices, texCoordIndices);
	this->textures = textures;
}

Mesh::~Mesh()
{
	//Memory::MemoryPool::release<Mesh>(this);
}

void Mesh::combineVertexData(
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

	for (size_t i = 0; i < posIndices.size(); ++i)
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

