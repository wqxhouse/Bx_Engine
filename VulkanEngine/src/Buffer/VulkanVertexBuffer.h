#pragma once

#include <Model/Mesh/Mesh.h>

class VulkanVertexBuffer
{
public:
    VulkanVertexBuffer();
    ~VulkanVertexBuffer();

private:
    std::vector<Vertex> m_vertices;
};