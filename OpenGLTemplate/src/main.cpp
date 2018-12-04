//=================================================================================================
//
//  Bx Engine
//  bxs3514 @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

/*
	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016
*/

#if BX_OPENGL
#include "OpenGLTemplate.h"
#elif BX_VULKAN
#include "Context\VulkanContext.h"
#endif

#include "Structure\LinkedList.h"

int main()
{

    Structure::LinkedList<int> linkedList;
    linkedList.insertBack(1);
    linkedList.insertBack(2);
    linkedList.clear();

    linkedList.insertBack(1);

	Setting setting;
    setting.m_graphicsSetting.DisableSSAO();
    setting.m_graphicsSetting.renderingMethod = FORWARD_PLUS_RENDERING;

#if BX_OPENGL
    OpenGLTemplate m_oglTemplate(&setting);
    m_oglTemplate.run();
#elif BX_VULKAN
    VulkanEngine::VulkanContext m_vulkanContext(&setting);
    m_vulkanContext.initialize();
    m_vulkanContext.run();
#endif
    
	return 0;
}
