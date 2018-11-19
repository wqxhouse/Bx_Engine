//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include "Framebuffer.h"
#include "../Shader/GraphicsShader.h"
#include "../Math/Vector3.h"
#include "../Material/Material.h"
#include "../Model/Primitives/Quad.h"

class Scene;

class GBuffer
{
public:
    GBuffer(
        Scene*     pScene,
        const UINT width,
        const UINT height);

    ~GBuffer();

    BOOL initialize();
    void drawGBuffer();

    void readGBuffer(const GLuint shaderProgram);
    void readGBuffer(
        const GLuint       shaderProgram,
        const std::string& textureName,
        const GLenum       texUnit);

    void draw();

    inline void UseGlobalMaterial() { useGlobalMaterial = TRUE;  }
    inline void UseLocalMaterial()  { useGlobalMaterial = FALSE; }

    inline Texture* GetGBufferTexture(const UINT i) const 
    { 
        return m_gFramebuffer.getTexturePtr(GL_TEXTURE0 + i);
    }

private:
    Scene*         m_pScene; // Pointer to the scene

    Framebuffer    m_gFramebuffer;
    GraphicsShader m_gShader;

    UINT m_width;
    UINT m_height;

    Quad m_gQuad;

    BOOL useGlobalMaterial;
};