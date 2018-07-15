#pragma once

#include "../Shader/Shader.h"
#include "../Texture/Texture.h"

class Skybox
{
public:
    Skybox(
        const std::vector<std::string>& skyboxImages);

    BOOL initialize();
    void draw();

    ~Skybox();

private:
    Shader m_skyboxShader;

    Cubemap m_skyboxCubemap;
};