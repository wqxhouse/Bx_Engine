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
    //setting.m_graphicsSetting.antialasing     = AA_VERY_HIGH;

#if BX_OPENGL
    OpenGLTemplate m_oglTemplate(&setting);
    m_oglTemplate.run();
#elif BX_VULKAN
    // Memory
    Memory::MemoryPool          m_memPool(256 * 1024 * 1024);
    Memory::MemoryPoolAllocator m_allocator(&m_memPool,
        DEFAULT_MAX_RENDER_SCENE_OBJ_NUM * DEFAULT_MAX_RENDER_SCENE_NUM,
        static_cast<UINT>(Scene::RenderScene::GetMaxObjSize()),
        0,
        Memory::Allocator::DEFAULT_ALIGNMENT_SIZE);

    Memory::MemoryPoolArena m_arena(&m_allocator);

    std::unique_ptr<Scene::RenderScene> m_pScene =
        std::unique_ptr<Scene::RenderScene>(new Scene::RenderScene(&setting, &m_arena, 4096));

#if _DEBUG
    m_pScene->AddObjModel("../resources/models/box/box.obj", "../resources/models/box/box.mtl",
                          &(Trans(Math::Vector3(-2.0f, 2.0f,  0.0f),
                                  Math::Vector3(0.0f, 0.0f, -1.0f),
                                  Math::Vector3(0.0f, 1.0f,  0.0f))));

    m_pScene->AddObjModel("../resources/models/sphere/sphere.obj", "../resources/models/sphere/sphere.mtl",
                          &(Trans(Math::Vector3(0.0f, 2.0f,  0.0f),
                                  Math::Vector3(0.0f, 0.0f, -1.0f),
                                  Math::Vector3(0.0f, 1.0f,  0.0f))));
#else
    m_pScene->AddObjModel("../resources/models/sponza/sponza_big.obj", "../resources/models/sponza/sponza_big.mtl",
                          &(Trans(Math::Vector3(2.0f, 2.0f,  0.0f),
                                  Math::Vector3(0.0f, 0.0f, -1.0f),
                                  Math::Vector3(0.0f, 1.0f,  0.0f))));

    m_pScene->AddObjModel("../resources/models/buddha/buddha.obj",
                          "../resources/models/buddha/buddha.mtl",
                          &(Trans(Math::Vector3(2.0f, -2.0f,  10.0f),
                                  Math::Vector3(0.0f, 0.0f, -1.0f),
                                  Math::Vector3(0.0f, 1.0f,  0.0f))));
#endif

    Trans prosCamTrans(Math::Vector3(0.0f, 2.0f, 20.0f),
                       Math::Vector3(0.0f, 0.0f, -1.0f),
                       Math::Vector3(0.0f, 1.0f, 0.0f));

    Object::Camera::ProspectiveCameraCreateInfo prosCamCreateInfo = {};
    prosCamCreateInfo.pTrans      = &prosCamTrans;
    prosCamCreateInfo.speed       = 20.0f;
    prosCamCreateInfo.aspectRatio = static_cast<float>(setting.resolution.width) /
                                    static_cast<float>(setting.resolution.height);
    prosCamCreateInfo.fov         = 70.0f;
    prosCamCreateInfo.nearClip    = 0.1f;
    prosCamCreateInfo.farClip     = 1000.0f;

    m_pScene->AddProspectiveCamera(prosCamCreateInfo);

    Trans lightTrans(Math::Vector3(0.0f, 2.0f, 20.0f),
                     Math::Vector3(0.0f, 0.0f, -1.0f),
                     Math::Vector3(0.0f, 1.0f, 0.0f));

    Object::Light::LightCreateInfo lightCreateInfo = {};
    lightCreateInfo.pTrans     = &lightTrans;
    lightCreateInfo.lightColor = Math::Vector3(1.0f, 1.0f, 1.0f);

    m_pScene->AddDirectionalLight(lightCreateInfo);

    VulkanEngine::VulkanContext m_vulkanContext(&setting, m_arena);
    m_vulkanContext.AddScene(m_pScene.get());
    m_vulkanContext.initialize();
    m_vulkanContext.run();
#endif
    
	return 0;
}
