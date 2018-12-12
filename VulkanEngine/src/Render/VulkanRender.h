//=================================================================================================
//
//  Bx Engine
//  bxs3514 (Xiangshun Bei) @ 2016 - 2018
//
//  All code licensed under the MIT license
//
//================================================================================================

#pragma once

#include <Scene/RenderScene.h>

namespace VulkanEngine
{
	namespace Render
	{
		class VulkanRenderBase
		{
		public:
			VulkanRenderBase(const Scene::RenderScene* pScene);
			~VulkanRenderBase();

			virtual void update(const float delta) = 0;
			virtual void render() = 0;

		protected:
			const Scene::RenderScene* m_pScene;
		};

		class VulkanForwardRender : public VulkanRenderBase
		{
		public:
			VulkanForwardRender(const Scene::RenderScene* pScene);
			~VulkanForwardRender();

			void update(const float delta);
			void render();

		private:

		};

		class VulkanDeferredRender : public VulkanRenderBase
		{
		public:
			VulkanDeferredRender(const Scene::RenderScene* pScene);
			~VulkanDeferredRender();

			void update(const float delta);
			void render();
		};
	}
}