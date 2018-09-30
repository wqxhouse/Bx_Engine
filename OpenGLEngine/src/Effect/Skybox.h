#pragma once

#include "../Shader/GraphicsShader.h"
#include "../Texture/Texture.h"
#include "../Model/Primitives/Cube.h"

class Scene;

class Skybox
{
public:
    Skybox(
        Scene*                          pScene,
        const std::vector<std::string>& skyboxImages);

    BOOL initialize();
    void draw();

    ~Skybox();

    inline Cubemap GetSkyboxCubemap() const { return m_skyboxCubemap; }

private:
    Scene*         m_pScene; // Pointer to the render scene

    GraphicsShader m_skyboxShader;

    Cubemap        m_skyboxCubemap;

    Cube           m_skyboxCube;
};