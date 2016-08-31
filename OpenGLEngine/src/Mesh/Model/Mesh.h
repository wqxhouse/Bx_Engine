#pragma once

#include "../../Core/OpenGLPCH.h"
#include "../../Math/Vector2.h"
#include "../../Math/Vector3.h"

class Mesh
{
public:
	struct Vertex
	{
		Math::Vector3 position;
		Math::Vector3 normal;
		Math::Vector2 texCoords;

		Vertex()
		{}

		Vertex(const Math::Vector3& position, const Math::Vector3& normal, const Math::Vector2& texCoords)
		{
			setData(position, normal, texCoords);
		}

		void setPositionData(const Math::Vector3& position)
		{
			this->position = position;
		}

		void setNormalData(const Math::Vector3& normal)
		{
			this->normal = normal;
		}

		void setTexCoords(const Math::Vector2& texCoords)
		{
			this->texCoords = texCoords;
		}

		void setData(const Math::Vector3& position, const Math::Vector3& normal, const Math::Vector2& texCoords)
		{
			this->position = position;
			this->normal = normal;
			this->texCoords = texCoords;
		}
	};

	struct Texture
	{
		GLuint id;
		std::string type;
	};
	
	Mesh(const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>& indices);

	Mesh(const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>& indices,
		const std::vector<Texture>& textures);

	Mesh(const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>& posIndices,
		const std::vector<GLuint>& normalIndices,
		const std::vector<GLuint>& texCoordIndices);

	Mesh(const std::vector<Math::Vector3>& posBuf, 
		 const std::vector<Math::Vector3>& normalBuf, 
		 const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>& posIndices,
		const std::vector<GLuint>& normalIndices,
		const std::vector<GLuint>& texCoordIndices,
		 const std::vector<Texture>& textures);

	~Mesh();

	std::vector<Vertex> vertexBuffer;
	std::vector<GLuint> indexBuffer;
	std::vector<Texture> textures;
private:
	void combineVertexData(
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>& posIndices,
		const std::vector<GLuint>& normalIndices,
		const std::vector<GLuint>& texCoordIndices);
};