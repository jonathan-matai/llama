#pragma once

#include "graphics/llrenderer.h"
#include "llwindowcontext_vk.h"


#include "graphics/llbuffer.h"

#include "math/llmath.h"

namespace llama
{
    class Shader_IVulkan;

    class Renderer_IVulkan : public Renderer_T, public std::enable_shared_from_this<Renderer_IVulkan>
    {
    public:

        Renderer_IVulkan(EventNode node, std::shared_ptr<GraphicsDevice_IVulkan> device, Window window);
        ~Renderer_IVulkan() override;

        //void tick() override;

        EventDispatchState onTick(TickEvent* e) override;

        void setShader(Shader shader, Shader shader2) override;

        inline vk::Device getDevice() const { return m_context->getDevice(); }
        inline vk::RenderPass getRenderPass() const { return m_context->getRenderPass(); }
        inline uint32_t getSwapchainSize() const { return static_cast<uint32_t>(m_context->m_frameBuffers.size()); }
        inline uint32_t getSwapchainIndex() const { return m_swapchainIndex; }
        inline std::shared_ptr<GraphicsDevice_IVulkan> getGraphicsDevice() const { return m_context->getGraphicsDevie(); }

    private:

        void recordCommandBuffers();

        bool recreateIfOutOfDate(vk::Result result, const DebugInfo& debugInfo, std::string_view message);

        struct Constants
        {
            float2 offset;
            float3x3 rotation;

            Constants(float2 o, float3x3 r) :
                offset(o),
                rotation(r)
            { }
        };

        std::shared_ptr<Shader_IVulkan> t_shader;

        VertexBuffer t_vertexBuffer;
        ConstantBuffer t_constantBuffer;
        ConstantSet t_constantSet;
        float t_colors;

        ConstantBuffer t_llamaConstants;
        VertexBuffer t_llamaVertex;
        Shader t_llamaShader;
        SampledImage t_llamaImage;
        ConstantSet t_llamaConstantSet;


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