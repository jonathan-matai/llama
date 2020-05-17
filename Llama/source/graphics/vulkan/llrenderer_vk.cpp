#include "llpch.h"
#include "llrenderer_vk.h"

#include "llshader_vk.h"
#include "llconstantset_vk.h"
#include "llbuffer_vk.h"

struct Vertex_T
{
    float x, y;
    float padding[2];
    float r, g, b;
    float morepadding;

    Vertex_T(float x, float y, float r, float g, float b) :
        x(x), y(y), r(r), g(g), b(b) { }
};

llama::Renderer_IVulkan::Renderer_IVulkan(EventNode node, std::shared_ptr<GraphicsDevice_IVulkan> device, Window window) :
    Renderer_T(node),
    m_context(std::make_unique<WindowContext_IVulkan>(window, device)),
    m_swapchainIndex(1),
    m_stopRenderer(node, this, &Renderer_IVulkan::stopRenderer)
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
}

llama::Renderer_IVulkan::~Renderer_IVulkan()
{
}

llama::EventDispatchState llama::Renderer_IVulkan::onTick(TickEvent* e)
{
    if (e->m_tickrateIndex != 0 || m_commandBuffers.empty())
        return EventDispatchState::IGNORED;

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

    return EventDispatchState::PROCESSED;
}

void llama::Renderer_IVulkan::addEntityManager(EntityManager manager)
{
    m_entityManagers.push_back(manager);
    recordCommandBuffers();
}

void llama::Renderer_IVulkan::unpackGroup(Group* group, vk::CommandBuffer buffer, uint32_t swapchainIndex)
{
    for (auto& a : *group)
    {
        if (a.second->m_flags.isSet(EntityFlags::RENDERABLE))
        {
            auto ro = static_cast<RenderableEntity*>(a.second.get());

            buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, std::static_pointer_cast<Shader_IVulkan>(ro->m_shader)->getPipeline());
            buffer.bindVertexBuffers(0, { std::static_pointer_cast<VertexBuffer_IVulkan>(ro->m_vertexBuffer)->getBuffer() }, { 0 });

            uint32_t offset = static_cast<uint32_t>(static_cast<ConstantBuffer_IVulkan*>(ro->m_constantBuffer->getBuffer())->offset(ro->m_arrayIndex, m_swapchainIndex));

            buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                      std::static_pointer_cast<Shader_IVulkan>(ro->m_shader)->getPipelineLayout(),
                                      0,
                                      { std::static_pointer_cast<ConstantSet_IVulkan>(ro->m_constantSet)->m_sets[swapchainIndex] },
                                      { offset });

            if (ro->m_indexBuffer)
            {
                auto ib = std::static_pointer_cast<IndexBuffer_IVulkan>(ro->m_indexBuffer);
                buffer.bindIndexBuffer(ib->getBuffer(), 0, ib->m_32bitIndices ? vk::IndexType::eUint32 : vk::IndexType::eUint16);
                buffer.drawIndexed(static_cast<uint32_t>(ib->m_indexCount), 1, 0, 0, 0);
            }
            else
            {
                buffer.draw(static_cast<uint32_t>(std::static_pointer_cast<VertexBuffer_IVulkan>(ro->m_vertexBuffer)->m_vertexCount), 1, 0, 0);
            }
        }
        else if (a.second->m_flags.isSet(EntityFlags::GROUP))
            unpackGroup(static_cast<Group*>(a.second.get()), buffer, swapchainIndex);
    }
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

        // START RENDERING

        for (auto a : m_entityManagers)
            unpackGroup(&a->m_entities, m_commandBuffers[i].get(), i);

        // END RENDERING

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

llama::EventDispatchState llama::Renderer_IVulkan::stopRenderer(CloseApplicationEvent* e)
{
    m_context->getDevice().waitIdle();
    m_commandBuffers.clear();

    return EventDispatchState::PROCESSED;
}

