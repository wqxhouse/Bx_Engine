/*

	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016
*/

#if BX_OPENGL
#include "OpenGLTemplate.h"
#elif BX_VULKAN
#include "Context\VulkanContext.h"
#endif

int main()
{
	Setting setting;
    setting.m_graphicsSetting.DisableSSAO();
    setting.m_graphicsSetting.renderingMethod = FORWARD_PLUS_RENDERING;

#if BX_OPENGL
    OpenGLTemplate m_oglTemplate(&setting);
    m_oglTemplate.run();
#elif BX_VULKAN
    VulkanContext m_vulkanContext(&setting);
    m_vulkanContext.initialize();
    m_vulkanContext.run();
#endif

	return 0;
}
