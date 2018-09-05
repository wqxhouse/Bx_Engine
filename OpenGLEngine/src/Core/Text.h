#pragma once

#include "../Shader/Shader.h"
#include "../Math/Vector3.h"
#include "../Texture/Texture.h"

class Scene;

class Text
{
public:
    Text();
    ~Text();

    void initialize();

    void RenderText(
        const Scene*         pScene,
        const std::string&   text,
        const Math::Vector2& pos,
        const float          textSize = 20.0f);

private:
    struct VertexUV
    {
        Math::Vector2 pos;
        Math::Vector2 uv;
    };
    
    VertexUV m_vertexUV[4];

    UINT indices[6] =
    {
        0, 1, 2,
        1, 3, 2
    };

    Shader m_textShader;

    GLuint m_vertexArrayObj;
    GLuint m_vertexBufObj;
    GLuint m_indexBufObj;

    Texture2D m_fontTexture;

    const float fontTexSize = 0.125f; // 1/8
};