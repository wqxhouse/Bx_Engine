#pragma once

#include <map>

#include "../../Context/Setting.h"

#include "../../Core/OpenGLPCH.h"
#include "../../Math/Vector2.h"
#include "../../Math/Vector3.h"
#include "../../Material/Material.h"
#include "../../Texture/Texture.h"
#include "../../Buffer/UniformBufferMgr.h"

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

		Vertex(
            const Math::Vector3& position,
            const Math::Vector3& normal,
            const Math::Vector2& texCoords)
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

		void setData(
            const Math::Vector3& position,
            const Math::Vector3& normal,
            const Math::Vector2& texCoords)
		{
			this->position = position;
			this->normal = normal;
			this->texCoords = texCoords;
		}

		bool operator<(const Vertex& vertex) const
		{
			return memcmp((void*)this, (void*)(&vertex), sizeof(Vertex)) < 0;
		}
	};
	
	Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>&        indices);

	Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>&        indices,
		const std::vector<Texture*>&      textures);

	Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>&        posIndices,
		const std::vector<GLuint>&        normalIndices,
		const std::vector<GLuint>&        texCoordIndices);

	Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf, 
		const std::vector<Math::Vector3>& normalBuf, 
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>&        posIndices,
		const std::vector<GLuint>&        normalIndices,
		const std::vector<GLuint>&        texCoordIndices,
		const std::vector<Texture*>&      textures);

    ~Mesh();

    void updateMaterial(
        UniformBufferMgr* pUniformBufferMgr,
        const GLuint      materialBufferIndex);

    void drawMeshPos();
    void draw();

    void updateVertexData();

    void* mapVertexBufferData();

    void setMaterial(Material* pMaterial);

    inline void UseGlobalMaterial() { useGlobalMaterial = TRUE; }
    inline void UseLocalMaterial()  { useGlobalMaterial = FALSE; }

    //inline GLuint GetVertexBufferObj() const { return m_vertexBufferObj; }

	std::string m_name;
	std::string m_materialName;

	std::vector<Vertex>   m_vertexBuffer;
	std::vector<GLuint>   m_indexBuffer;
	std::vector<Texture*> m_textureList;

	Material* m_pMaterial;

private:
	void initialize();

	void combineVertexData(
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<GLuint>&        posIndices,
		const std::vector<GLuint>&        normalIndices,
		const std::vector<GLuint>&        texCoordIndices);

	bool findSimilarVertex(const std::map<Vertex, GLuint>& map,
                           const Vertex& vertex, GLuint* index);

	PolyMode m_polyMode = TRIANGLE;

	GLuint m_vertexArrayObj;  // VAO
	GLuint m_vertexBufferObj; // VBO

	GLuint m_indexBufferObj;

    UniformBufferMgr* m_pUniformBufferMgr;
    GLuint            m_materialBufferIndex;

    BOOL useGlobalMaterial;
};