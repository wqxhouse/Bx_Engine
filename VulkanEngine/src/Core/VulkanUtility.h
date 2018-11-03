#pragma once

#include "VulkanPCH.h"
#include "Utility.h"

class VulkanUtility
{
public:
    static BOOL CheckValidationLayerSupport();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		severiry,
	VkDebugUtilsMessageTypeFlagsEXT				type,
	const VkDebugUtilsMessengerCallbackDataEXT* pData,
	void*										pUserData);

    static inline BOOL IsValidationLayerEnabled() { return m_enableValidationLayer; }

private:
	std::vector<const char*> getRequiredExts();

    const static std::vector<const char*> m_validationLayers;

#if _DEBUG
    const static BOOL m_enableValidationLayer = TRUE;
#elif
    const static bool m_enableValidationLayer = FALSE;
#endif
};
