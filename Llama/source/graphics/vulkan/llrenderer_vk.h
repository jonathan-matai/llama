#pragma once

#include "graphics/llrenderer.h"
#include "llwindowcontext_vk.h"


#include "graphics/llbuffer.h"

#include "math/llmath.h"

namespace llama
{
    class Shader_IVulkan;

    class Renderer_IVulkan : public Renderer_T
    {
    public:

        Renderer_IVulkan(EventNode node, std::shared_ptr<GraphicsDevice_IVulkan> device, Window window);
        ~Renderer_IVulkan() override;

        EventDispatchState onTick(TickEvent* e) override;

        void addEntityManager(EntityManager manager) override;

        inline vk::Device getDevice() const { return m_context->getDevice(); }
        inline vk::RenderPass getRenderPass() const { return m_context->getRenderPass(); }
        inline uint32_t getSwapchainSize() const { return static_cast<uint32_t>(m_context->m_frameBuffers.size()); }
        inline uint32_t getSwapchainIndex() const { return m_swapchainIndex; }
        inline std::shared_ptr<GraphicsDevice_IVulkan> getGraphicsDevice() const { return m_context->getGraphicsDevie(); }

    private:

        void unpackGroup(Group* group, vk::CommandBuffer buffer, uint32_t swapchainIndex);
        void recordCommandBuffers();

        bool recreateIfOutOfDate(vk::Result result, const DebugInfo& debugInfo, std::string_view message);

        EventDispatchState stopRenderer(CloseApplicationEvent* e);

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

        std::vector<EntityManager> m_entityManagers;

        EventDispatchFunction m_stopRenderer;
    };
}