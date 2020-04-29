#pragma once

#include "graphics/llrenderer.h"
#include "llwindowcontext_vk.h"

#include "llshader_vk.h"

namespace llama
{
    class Renderer_IVulkan : public Renderer_T
    {
    public:

        Renderer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, Window window);
        ~Renderer_IVulkan() override;

        void tick() override;

        void setShader(Shader shader) override;

        vk::Device getDevice() const { return m_context->getDevice(); }
        vk::RenderPass getRenderPass() const { return m_context->getRenderPass(); }

    private:

        void recordCommandBuffers();

        std::shared_ptr<Shader_IVulkan> t_shader;

        struct SyncObjects
        {
            vk::UniqueSemaphore renderSemaphore;
            vk::UniqueSemaphore presentSemaphore;
            vk::UniqueFence fence;
        };

        std::vector<SyncObjects> m_syncObjects;
        std::vector<vk::UniqueCommandBuffer> m_commandBuffers;
        uint32_t m_swapchainIndex;

        std::unique_ptr<WindowContext_IVulkan> m_context;
    };
}