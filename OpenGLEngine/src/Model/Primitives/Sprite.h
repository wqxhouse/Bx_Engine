#pragma once

#include "../../Math/Vector3.h"
#include "../../Texture/Texture.h"
#include "../../Shader/Shader.h"
#include "../../Context/Setting.h"

struct RectangleSize
{
    float width;
    float height;
};

class Scene;

struct VertexUV
{
    Math::Vector3 pos;
    Math::Vector2 uv;
};

class Sprite
{
public:
    Sprite(
        Scene*               pScene,
        const Math::Vector3& spritePosition,
        const RectangleSize& spriteSize);

    ~Sprite();

    BOOL initialize();
    void draw();

    void BindTexture(Texture2D* pSpriteTexture);

    inline Math::Vector3 GetPosition() const { return m_spritePosition; }

private:
    Math::Vector3 m_spritePosition;
    RectangleSize m_spriteSize;
    Texture2D*    m_pSpriteTexture;

    Shader m_spriteShader;

    GLuint m_vertexArrayObj;
    GLuint m_vertexBufObj;
    GLuint m_indexBufObj;

    UINT m_spriteIndices[6] =
    {
        0, 1, 2,
        1, 3, 2
    };

    VertexUV m_spriteVertices[4];
};