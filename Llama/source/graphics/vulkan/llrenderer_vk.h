#pragma once

#include "graphics/llrenderer.h"
#include "llshader_vk.h"

namespace llama
{
    class Renderer_IVulkan : public Renderer_T
    {
    public:

        Renderer_IVulkan(std::shared_ptr<WindowContext_IVulkan> context, std::shared_ptr<Shader_IVulkan> shader);
        ~Renderer_IVulkan() override;

        void tick() override;
    private:

        struct SyncObjects
        {
            vk::UniqueSemaphore renderSemaphore;
            vk::UniqueSemaphore presentSemaphore;
            vk::UniqueFence fence;
        };

        std::vector<SyncObjects> m_syncObjects;
        std::vector<vk::UniqueCommandBuffer> m_commandBuffers;
        uint32_t m_swapchainIndex;

        std::shared_ptr<WindowContext_IVulkan> m_context;
    };
}