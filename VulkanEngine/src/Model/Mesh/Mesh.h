#pragma once

#include <map>

#include "../../Context/Setting.h"

#include "../../Core/VulkanPCH.h"
#include "../../Math/Vector2.h"
#include "../../Math/Vector3.h"
#include "../Material/Material.h"

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
	
    struct VulkanVertex : public Vertex
    {
        
    };

	Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<UINT>&          indices);

	/*Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<UINT>&          indices,
		const std::vector<Texture*>&      textures);*/

	Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<UINT>&          posIndices,
		const std::vector<UINT>&          normalIndices,
		const std::vector<UINT>&          texCoordIndices);

	/*Mesh(
		const std::string&                name,
		const std::string&                materialFile,
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf, 
		const std::vector<Math::Vector3>& normalBuf, 
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<UINT>&          posIndices,
		const std::vector<UINT>&          normalIndices,
		const std::vector<UINT>&          texCoordIndices,
		const std::vector<Texture*>&      textures);*/

    ~Mesh();

    /*void updateMaterial(
        UniformBufferMgr* pUniformBufferMgr,
        const UINT      materialBufferIndex);

    void drawMeshPos(const UINT shaderProgram);
    void draw(const UINT shaderProgram);*/

    void updateVertexData();

    void* mapVertexBufferData();

    void setMaterial(Material* pMaterial);
    inline void setMaterialMap(MaterialMap* pMaterialMap)
    {
        m_pMaterialMap = pMaterialMap;
    }

    inline Material*    GetMaterial()    { return m_pMaterial;    }
    inline MaterialMap* GetMaterialMap() { return m_pMaterialMap; }

    inline void UseGlobalMaterial() { useGlobalMaterial = TRUE; }
    inline void UseLocalMaterial()  { useGlobalMaterial = FALSE; }

    //inline UINT GetVertexBufferObj() const { return m_vertexBufferObj; }

    void AddTexture(
        const std::string&           textureFile,
        const MESH_TEXTURE_MAP_TYPES type);

    std::vector<Vertex>   m_vertexBuffer;
    std::vector<UINT>   m_indexBuffer;

private:
	// void initialize();

	void combineVertexData(
		const int                         counter[],
		const std::vector<Math::Vector3>& posBuf,
		const std::vector<Math::Vector3>& normalBuf,
		const std::vector<Math::Vector2>& texCoords,
		const std::vector<UINT>&        posIndices,
		const std::vector<UINT>&        normalIndices,
		const std::vector<UINT>&        texCoordIndices);

	bool findSimilarVertex(const std::map<Vertex, UINT>& map,
                           const Vertex& vertex, UINT* index);

	PolyMode m_polyMode = POLYMODE_FILL;

    std::string m_name;
    std::string m_materialName;

    Material*             m_pMaterial;
    MaterialMap*          m_pMaterialMap;

	UINT m_vertexArrayObj;  // VAO
	UINT m_vertexBufferObj; // VBO

	UINT m_indexBufferObj;

    //UniformBufferMgr* m_pUniformBufferMgr;
    UINT            m_materialBufferIndex;

    BOOL useGlobalMaterial;
};