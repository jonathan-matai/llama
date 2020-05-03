#pragma once

#include "graphics/llrenderer.h"
#include "llwindowcontext_vk.h"


#include "llbuffer_vk.h"

namespace llama
{
    class Shader_IVulkan;

    class Renderer_IVulkan : public Renderer_T
    {
    public:

        Renderer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, Window window);
        ~Renderer_IVulkan() override;

        void tick() override;

        void setShader(Shader shader) override;

        inline vk::Device getDevice() const { return m_context->getDevice(); }
        inline vk::RenderPass getRenderPass() const { return m_context->getRenderPass(); }
        inline uint32_t getSwapchainSize() const { return static_cast<uint32_t>(m_context->m_frameBuffers.size()); }

    private:

        void recordCommandBuffers();

        bool recreateIfOutOfDate(vk::Result result, const DebugInfo& debugInfo, std::string_view message);

        std::shared_ptr<Shader_IVulkan> t_shader;

        VertexBuffer t_vertexBuffer;
        ConstantBuffer t_constantBuffer;
        ConstantSet t_constantSet;
        float t_colors;

        struct SyncObjects
        {
            vk::UniqueSemaphore renderSemaphore;
            vk::UniqueSemaphore presentSemaphore;
            vk::UniqueFence fence;
        };
        std::vector<SyncObjects> m_syncObjects;
        vk::UniqueSemaphore m_swapSemaphore;

        std::vector<vk::UniqueCommandBuffer> m_commandBuffers;
        uint32_t m_swapchainIndex;

        std::unique_ptr<WindowContext_IVulkan> m_context;
    };
}