#include "../../Memory/MemoryPool.h"

#include "Mesh.h"

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
	int count = posIndices.size();
	vertexBuffer.assign(posBuf.size(), Vertex());

	for (int i = 0; i < count; ++i)
	{
		int posIndex = posIndices[i];
		int normalIndex = normalIndices[i];
		int uvIndex = texCoordIndices[i];

		if (posIndices.size() == 0)
		{
			throw std::exception("Can't load the mesh without vertexes.\n");
		}

		vertexBuffer[posIndex].setPositionData(posBuf[posIndex]);

		if (normalIndices.size() != 0)
		{
			vertexBuffer[posIndex].setNormalData(normalBuf[normalIndex]);
		}

		if (texCoords.size() != 0)
		{
			vertexBuffer[posIndex].setTexCoords(texCoords[uvIndex]);
		}
	}

	this->indexBuffer = posIndices;
}

