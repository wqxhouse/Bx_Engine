#pragma once

#include "../Core/VulkanPCH.h"
#include "Setting.h"

class VulkanContext
{
public:
    VulkanContext(Setting* pSetting);
    ~VulkanContext();

    void initialize();
    void run();

    inline Setting* GetSetting() const   { return m_pSetting; }
    inline GLFWwindow* GetWindow() const { return m_pWindow;  }

private:
    BOOL initWindow();
    BOOL initVulkan();
    BOOL createInstance();

    // Window pointer and settings
    Setting*    m_pSetting;
    GLFWwindow* m_pWindow;

    // Timer
    float prevTime  = 0.0f;
    float deltaTime = 0.0f;

    // Vulkan library members
    const std::vector<const char* const> m_validationLayers =
    {
        "VK_LAYER_LUNARG_standard_validation"
    };

#if _DEBUG
    const bool m_enableValidationLayer = TRUE;

    bool checkValidationLayerSupport();

#elif
    const bool enableValidationLayer = FALSE;
#endif

    VkInstance m_vkInstance;

};