#include "llpch.h"
#include "llrenderer_vk.h"

#include "llshader_vk.h"
#include "llconstantset_vk.h"

#include "math/llmath.h"

struct Vertex_T
{
    float x, y;
    float padding[2];
    float r, g, b;
    float morepadding;

    Vertex_T(float x, float y, float r, float g, float b) :
        x(x), y(y), r(r), g(g), b(b) { }
};

llama::Renderer_IVulkan::Renderer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, Window window) :
    m_context(std::make_unique<WindowContext_IVulkan>(window, device)),
    m_swapchainIndex(1)
{
    m_syncObjects.resize(m_context->m_frameBuffers.size());

    for (auto& a : m_syncObjects)
    {
        assert_vulkan(m_context->getDevice().createSemaphoreUnique(vk::SemaphoreCreateInfo()),
                      a.renderSemaphore, LLAMA_DEBUG_INFO, "vk::Device::createSemaphoreUnique() failed!");

        assert_vulkan(m_context->getDevice().createSemaphoreUnique(vk::SemaphoreCreateInfo()),
                      a.presentSemaphore, LLAMA_DEBUG_INFO, "vk::Device::createSemaphoreUnique() failed!");

        assert_vulkan(m_context->getDevice().createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)),
                      a.fence, LLAMA_DEBUG_INFO, "vk::Device::createFenceUnique() failed!");
    }

    assert_vulkan(m_context->getDevice().createSemaphoreUnique(vk::SemaphoreCreateInfo()),
                  m_swapSemaphore, LLAMA_DEBUG_INFO, "vk::Device::createSemaphoreUnique() failed!");
    
    {
        std::lock_guard lock(m_context->m_device->getGraphicsQueue().queueFamily->m_commandPoolMutex);

        vk::CommandPool pool = m_context->m_device->getGraphicsQueue().queueFamily->m_commandPool.get();

        assert_vulkan(m_context->getDevice().allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(pool,
                                                                                                        vk::CommandBufferLevel::ePrimary,
                                                                                                        static_cast<uint32_t>(m_syncObjects.size()))),
                      m_commandBuffers, LLAMA_DEBUG_INFO, "vk::Device::allocateCommandBuffersUnique() failed!");
    }

    auto result = m_context->getDevice().acquireNextImageKHR(m_context->m_swapchain.get(), UINT64_MAX, m_swapSemaphore.get(), nullptr);

    if (recreateIfOutOfDate(result.result, LLAMA_DEBUG_INFO, "vk::Device::acquireNextImageKHR() failed!"))
        result = m_context->getDevice().acquireNextImageKHR(m_context->m_swapchain.get(), UINT64_MAX, m_swapSemaphore.get(), nullptr);

    m_swapchainIndex = result.value;
    std::swap(m_swapSemaphore, m_syncObjects[m_swapchainIndex].renderSemaphore);

    // Wait for previous frame at Index to finish rendering

    assert_vulkan(m_context->getDevice().waitForFences({ m_syncObjects[m_swapchainIndex].fence.get() }, VK_TRUE, UINT64_MAX),
                  LLAMA_DEBUG_INFO, "vk::Device::waitForFences() has failed!");

    assert_vulkan(m_context->getDevice().resetFences({ m_syncObjects[m_swapchainIndex].fence.get() }),
                  LLAMA_DEBUG_INFO, "vk::Device::resetFences() has failed!");

    std::vector<Vertex_T> verticies
    {
        Vertex_T(0.0f, 0.0f, 1, 0, 0),
        Vertex_T(0.0f, -.5f, 0, 1, 0),
        Vertex_T(0.5f, 0.0f, 0, 0, 1),

        Vertex_T(0.0f, 0.0f, 1, 0, 0),
        Vertex_T(0.5f, 0.0f, 0, 1, 0),
        Vertex_T(0.0f, 0.5f, 0, 0, 1),

        Vertex_T(0.0f, 0.0f, 1, 0, 0),
        Vertex_T(0.0f, 0.5f, 0, 1, 0),
        Vertex_T(-.5f, 0.0f, 0, 0, 1),

        Vertex_T(0.0f, 0.0f, 1, 0, 0),
        Vertex_T(-.5f, 0.0f, 0, 1, 0),
        Vertex_T(0.0f, -.5f, 0, 0, 1),
    };

    t_vertexBuffer = std::make_shared<VertexBuffer_IVulkan>(device, sizeof(Vertex_T) * verticies.size(), verticies.data());
    t_constantBuffer = std::make_shared<ConstantBuffer_IVulkan>(device, sizeof(float), 1, getSwapchainSize());

    t_colors = 0.0f;


    std::vector<float4> llamaVerticies
    {
        float4(-.15f, -.3f, 0.0f, 0.0f),
        float4(0.15f, -.3f, 1.0f, 0.0f),
        float4(0.15f, 0.3f, 1.0f, 1.0f),
        float4(0.15f, 0.3f, 1.0f, 1.0f),
        float4(-.15f, 0.3f, 0.0f, 1.0f),
        float4(-.15f, -.3f, 0.0f, 0.0f),
    };

    struct Constants
    {
        float2 offset;
        float3x3 rotation;

        Constants(float2 o, float3x3 r) :
            offset(o),
            rotation(r)
        { }
    };

    t_llamaVertex = createVertexBuffer(device, sizeof(float4) * llamaVerticies.size(), llamaVerticies.data());

    t_llamaConstants = createConstantBuffer(device, sizeof(Constants), 4);
    *static_cast<Constants*>(t_llamaConstants->at(0)) = Constants(float2(-.5, -.5), float3x3());
    *static_cast<Constants*>(t_llamaConstants->at(1)) = Constants(float2(0.5, -.5), float3x3());
    *static_cast<Constants*>(t_llamaConstants->at(2)) = Constants(float2(0.5, 0.5), float3x3());
    *static_cast<Constants*>(t_llamaConstants->at(3)) = Constants(float2(-.5, 0.5), float3x3());

    t_llamaImage = llama::createSampledImage(device, "resources/textures/llama.png");
}

llama::Renderer_IVulkan::~Renderer_IVulkan()
{
    m_context->getDevice().waitIdle();
}

void llama::Renderer_IVulkan::tick()
{
    if ((t_colors += 0.001f) > 3.0f)
        t_colors -= 3.0f;

    *static_cast<float*>(t_constantBuffer->at(0, m_swapchainIndex)) = t_colors;

    // Submit to Queue

    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    assert_vulkan(m_context->m_device->getGraphicsQueue().queueHandle.submit({ vk::SubmitInfo(1, &m_syncObjects[m_swapchainIndex].renderSemaphore.get(), &flags,
                                                                                              1, &m_commandBuffers[m_swapchainIndex].get(),
                                                                                              1, &m_syncObjects[m_swapchainIndex].presentSemaphore.get()) }, 
                                                                             m_syncObjects[m_swapchainIndex].fence.get()),
                  LLAMA_DEBUG_INFO, "vk::Queue::submit() has failed!");

    // Present

    vk::Result r = m_context->m_device->getGraphicsQueue().queueHandle.presentKHR(vk::PresentInfoKHR(1, &m_syncObjects[m_swapchainIndex].presentSemaphore.get(),
                                                                                                     1, &m_context->m_swapchain.get(), &m_swapchainIndex));
    
    recreateIfOutOfDate(r, LLAMA_DEBUG_INFO, "vk::Queue::presentKHR() failed!");
   
    // Acquire next Image

    auto result = m_context->getDevice().acquireNextImageKHR(m_context->m_swapchain.get(), UINT64_MAX, m_swapSemaphore.get(), nullptr);

    if (recreateIfOutOfDate(result.result, LLAMA_DEBUG_INFO, "vk::Device::acquireNextImageKHR() failed!"))
        result = m_context->getDevice().acquireNextImageKHR(m_context->m_swapchain.get(), UINT64_MAX, m_swapSemaphore.get(), nullptr);

    m_swapchainIndex = result.value;
    std::swap(m_swapSemaphore, m_syncObjects[m_swapchainIndex].renderSemaphore);

    // Wait for previous frame at Index to finish rendering

    assert_vulkan(m_context->getDevice().waitForFences({ m_syncObjects[m_swapchainIndex].fence.get() }, VK_TRUE, UINT64_MAX),
                  LLAMA_DEBUG_INFO, "vk::Device::waitForFences() has failed!");

    assert_vulkan(m_context->getDevice().resetFences({ m_syncObjects[m_swapchainIndex].fence.get() }),
                  LLAMA_DEBUG_INFO, "vk::Device::resetFences() has failed!");
}

void llama::Renderer_IVulkan::setShader(Shader shader, Shader shader2)
{
    t_shader = std::static_pointer_cast<Shader_IVulkan>(shader);
    t_llamaShader = shader2;

    t_constantSet = createConstantSet(t_shader, 0, { t_constantBuffer });
    t_llamaConstantSet = createConstantSet(t_llamaShader, 0, { t_llamaConstants, t_llamaImage });

    recordCommandBuffers();
}

void llama::Renderer_IVulkan::recordCommandBuffers()
{
    std::lock_guard lock(m_context->m_device->getGraphicsQueue().queueFamily->m_commandPoolMutex);

    for (uint32_t i = 0; i < m_commandBuffers.size(); ++i)
    {
        assert_vulkan(m_commandBuffers[i]->begin(vk::CommandBufferBeginInfo()),
                      LLAMA_DEBUG_INFO, "vk::CommandBuffer:begin() failed!");

        std::array<vk::ClearValue, 3> clearValues
        {
            vk::ClearValue(), // Ignored
            vk::ClearColorValue(std::array<float, 4>{ 0.06f, 0.06f, 0.06f, 1.0f }),
            vk::ClearDepthStencilValue(1.0f, 0)
        };

        vk::RenderPassBeginInfo beginInfo(m_context->getRenderPass(),
                                          m_context->m_frameBuffers[i].get(),
                                          vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(m_context->m_swapchainWidth, m_context->m_swapchainHeight)),
                                          static_cast<uint32_t>(clearValues.size()), clearValues.data());

        m_commandBuffers[i]->beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

        m_commandBuffers[i]->setViewport(0, { vk::Viewport(0.0f, // xOffset
                                                           0.0f, // yOfffset
                                                           static_cast<float>(m_context->m_swapchainWidth), // Width
                                                           static_cast<float>(m_context->m_swapchainHeight), // Height
                                                           0.0f,
                                                           1.0f) });

        m_commandBuffers[i]->setScissor(0, { beginInfo.renderArea });

        m_commandBuffers[i]->bindVertexBuffers(0, { std::static_pointer_cast<VertexBuffer_IVulkan>(t_vertexBuffer)->getBuffer() }, { 0 });
        m_commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, t_shader->getPipeline());
        m_commandBuffers[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, 
                                                t_shader->getPipelineLayout(), 
                                                0, // First Set
                                                1, &std::static_pointer_cast<ConstantSet_IVulkan>(t_constantSet)->m_sets[i].get(), // Sets
                                                0, nullptr /* Dynamic Offsets */);
        m_commandBuffers[i]->draw(12, 1, 0, 0);

        m_commandBuffers[i]->bindVertexBuffers(0, { std::static_pointer_cast<VertexBuffer_IVulkan>(t_llamaVertex)->getBuffer() }, { 0 });
        m_commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, std::static_pointer_cast<Shader_IVulkan>(t_llamaShader)->getPipeline());

        for (uint32_t j = 0; j < 4; ++j)
        {
            uint32_t offset = static_cast<uint32_t>(t_llamaConstants->offset(j));

            m_commandBuffers[i]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                    std::static_pointer_cast<Shader_IVulkan>(t_llamaShader)->getPipelineLayout(),
                                                    0,
                                                    1, &std::static_pointer_cast<ConstantSet_IVulkan>(t_llamaConstantSet)->m_sets[i].get(),
                                                    1, &offset);

            m_commandBuffers[i]->draw(6, 1, 0, 0);
        }


        m_commandBuffers[i]->endRenderPass();
        m_commandBuffers[i]->end();

        logfile()->print(Colors::WHITE, "Updated CommandBuffer %p", m_commandBuffers[i].get());
    }
}


bool llama::Renderer_IVulkan::recreateIfOutOfDate(vk::Result result, const DebugInfo& debugInfo, std::string_view message)
{
    if (result == vk::Result::eSuccess)
        return false;
    else if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
    {
        assert_vulkan(m_context->getDevice().waitIdle(),
                      LLAMA_DEBUG_INFO, "vk::Device::waitIdle() failed!");

        m_context->recreate();
        recordCommandBuffers();
        return true;
    }
    else
        assert_vulkan(result, debugInfo, message);

    return false;
}

