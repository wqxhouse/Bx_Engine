#include "VulkanShader.h"

VulkanShader::VulkanShader(
    const VkDevice& device)
    : m_device(device)
{
    // vkCreateShaderModule(device, NULL, NULL, NULL);
}

VulkanShader::~VulkanShader()
{
}

void VulkanShader::PreprocessingGLSL(
    const std::string& glslFileName)
{
}

void VulkanShader::CompileGLSL(
    const std::string& glslFileName,
    const BOOL         preprocess)
{
}
