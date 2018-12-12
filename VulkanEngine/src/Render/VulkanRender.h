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
#include "../Shader/VulkanGraphicsShader.h"

namespace VulkanEngine
{
	namespace Render
	{
		class VulkanRenderBase
		{
		public:
			VulkanRenderBase(const Scene::RenderScene*		     pScene,
							 const Shader::VulkanGraphicsShader* pShader);

			~VulkanRenderBase();

			virtual void update(const float delta) = 0;
			virtual void draw() = 0;

		protected:
			const Scene::RenderScene*		   m_pScene;
			const Shader::VulkanGraphicsShader m_pShader;
		};

		class VulkanForwardRender : public VulkanRenderBase
		{
		public:
			VulkanForwardRender(const Scene::RenderScene*			pScene,
								const Shader::VulkanGraphicsShader* pShader);

			~VulkanForwardRender();

			void update(const float delta);
			void draw();

		private:

		};

		class VulkanDeferredRender : public VulkanRenderBase
		{
		public:
			VulkanDeferredRender(const Scene::RenderScene*		     pScene,
								 const Shader::VulkanGraphicsShader* pShader);

			~VulkanDeferredRender();

			void update(const float delta);
			void draw();
		};
	}
}