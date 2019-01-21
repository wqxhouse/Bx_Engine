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

int main()
{
	Setting setting;
    setting.m_graphicsSetting.DisableSSAO();
    setting.m_graphicsSetting.renderingMethod = FORWARD_PLUS_RENDERING;

    // Memory
    Memory::MemoryPool          m_memPool(256 * 1024 * 1024);
    Memory::MemoryPoolAllocator m_allocator(&m_memPool,
                                            DEFAULT_MAX_RENDER_SCENE_OBJ_NUM * DEFAULT_MAX_RENDER_SCENE_NUM,
                                            static_cast<UINT>(Scene::RenderScene::GetMaxObjSize()),
                                            0,
                                            Memory::Allocator::DEFAULT_ALIGNMENT_SIZE);

    Memory::MemoryPoolArena     m_arena(&m_allocator);

#if BX_OPENGL
    OpenGLTemplate m_oglTemplate(&setting);
    m_oglTemplate.run();
#elif BX_VULKAN
    VulkanEngine::VulkanContext m_vulkanContext(&setting, m_arena);
    m_vulkanContext.initialize();
    m_vulkanContext.run();
#endif
    
	return 0;
}
