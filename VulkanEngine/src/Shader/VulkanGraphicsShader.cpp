#include "VulkanGraphicsShader.h"

VulkanGraphicsShader::VulkanGraphicsShader(
    const VkDevice& device)
    : VulkanShader(device)
{
    m_shaderSources.resize(BX_MAX_SHADER);

    m_shaderFlags.value = 0;
}

VulkanGraphicsShader::~VulkanGraphicsShader()
{
}

BOOL VulkanGraphicsShader::createPipelineShaderStages(
    const BxShaderMeta& shaderMeta)
{
    BOOL result = BX_SUCCESS;

    // Vertex Shader
    const std::string& vsFile = shaderMeta.vertexShaderInfo.shaderFile;
    if (vsFile.empty() == FALSE)
    {
        m_shaderFlags.vertexShaderBit = TRUE;
        VDeleter<VkShaderModule> vertexShaderModule =
            createShaderModule(vsFile, BX_VERTEX_SHADER);

        m_shaderStages.push_back(
            createShaderStage(vertexShaderModule, shaderMeta.vertexShaderInfo.shaderEntry));
    }

    // Tessellation
    const std::string& tesFile = shaderMeta.tesShaderInfo.shaderFile;
    if (tesFile.empty() == FALSE)
    {
        m_shaderFlags.tesShaderBit = TRUE;
        VDeleter<VkShaderModule> tesShaderModule =
            createShaderModule(tesFile, BX_TES_SHADER);

        m_shaderStages.push_back(
            createShaderStage(tesShaderModule, shaderMeta.tesShaderInfo.shaderEntry));
    }

    const std::string& tcsFile = shaderMeta.tcsShaderInfo.shaderFile;
    if (tcsFile.empty() == FALSE)
    {
        m_shaderFlags.tcsShaderBit = TRUE;
        VDeleter<VkShaderModule> tcsShaderModule =
            createShaderModule(tcsFile, BX_TCS_SHADER);

        m_shaderStages.push_back(
            createShaderStage(tcsShaderModule, shaderMeta.tcsShaderInfo.shaderEntry));
    }

    // Geometry shader
    const std::string& gsFile = shaderMeta.geometryShaderInfo.shaderFile;
    if (gsFile.empty() == FALSE)
    {
        m_shaderFlags.geometryShaderBit = TRUE;
        VDeleter<VkShaderModule> geometryShaderModule =
            createShaderModule(gsFile, BX_GEOMETRY_SHADER);

        m_shaderStages.push_back(
            createShaderStage(geometryShaderModule, shaderMeta.geometryShaderInfo.shaderEntry));
    }

    // Fragment(Pixel) shader
    const std::string& fsFile = shaderMeta.fragmentShaderInfo.shaderFile;
    if (fsFile.empty() == FALSE)
    {
        m_shaderFlags.fragmentShaderBit = TRUE;

        VDeleter<VkShaderModule> fragmentShaderModule =
            createShaderModule(fsFile, BX_FRAGMENT_SHADER);

        m_shaderStages.push_back(
            createShaderStage(fragmentShaderModule, shaderMeta.fragmentShaderInfo.shaderEntry));
    }

    return result;
}

VkShaderModule VulkanGraphicsShader::createShaderModule(
    const std::string&   shaderFile,
    const BX_SHADER_TYPE shaderType)
{
    VDeleter<VkShaderModule> shaderModule = { m_device, vkDestroyShaderModule };

    m_shaderSources[shaderType] = { shaderType, VulkanUtility::ReadFile(shaderFile, TRUE) };

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = m_shaderSources[shaderType].source.size();
    shaderModuleCreateInfo.pCode    = reinterpret_cast<const UINT*>(
                                      m_shaderSources[shaderType].source.data());

    VkResult result = vkCreateShaderModule(
        m_device, &shaderModuleCreateInfo, NULL, shaderModule.replace());

    assert(result == VK_SUCCESS);

    return shaderModule;
}

VkPipelineShaderStageCreateInfo VulkanGraphicsShader::createShaderStage(
    const VkShaderModule& shaderModule,
    const std::string&    shaderEntry)
{
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
    shaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo.module = shaderModule;
    shaderStageCreateInfo.pName  = shaderEntry.data();

    return shaderStageCreateInfo;
}
