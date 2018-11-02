#pragma once

#include "VulkanPCH.h"
#include "Utility.h"

class VulkanUtility
{
public:
    static BOOL CheckValidationLayerSupport();
    static inline BOOL IsValidationLayerEnabled() { return m_enableValidationLayer; }

private:
    const static std::vector<const char*> m_validationLayers;

#if _DEBUG
    const static BOOL m_enableValidationLayer = TRUE;
#elif
    const static bool m_enableValidationLayer = FALSE;
#endif
};
