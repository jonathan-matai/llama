#include "llpch.h"
#include "llrenderer_vk.h"

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

    std::lock_guard lock(m_context->m_device->getGraphicsQueue().queueFamily->m_commandPoolMutex);

    vk::CommandPool pool = m_context->m_device->getGraphicsQueue().queueFamily->m_commandPool.get();

    assert_vulkan(m_context->getDevice().allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(pool, 
                                                                                                  vk::CommandBufferLevel::ePrimary, 
                                                                                                  static_cast<uint32_t>(m_syncObjects.size()))),
                  m_commandBuffers, LLAMA_DEBUG_INFO, "vk::Device::allocateCommandBuffersUnique() failed!");
}

llama::Renderer_IVulkan::~Renderer_IVulkan()
{
    m_context->getDevice().waitIdle();
}

void llama::Renderer_IVulkan::tick()
{

    uint32_t nextIndex = 0;

    auto result = m_context->getDevice().acquireNextImageKHR(m_context->m_swapchain.get(), UINT64_MAX, m_syncObjects[m_swapchainIndex].renderSemaphore.get(), nullptr);

    if (result.result == vk::Result::eSuccess)
        nextIndex = result.value;
    else if (result.result == vk::Result::eSuboptimalKHR || result.result == vk::Result::eErrorOutOfDateKHR)
    {
        assert_vulkan(m_context->getDevice().waitIdle(),
                      LLAMA_DEBUG_INFO, "vk::Device::waitIdle() failed!");

        m_context->recreate();
        recordCommandBuffers();
        return;
    }
    else
        assert_vulkan(result.result, LLAMA_DEBUG_INFO, "vk::Device::acquireNextImageKHR() failed!");

    // !!!!
    std::swap(nextIndex, m_swapchainIndex);

    assert_vulkan(m_context->getDevice().waitForFences({ m_syncObjects[m_swapchainIndex].fence.get() }, VK_TRUE, UINT64_MAX),
                  LLAMA_DEBUG_INFO, "vk::Device::waitForFences() has failed!");

    assert_vulkan(m_context->getDevice().resetFences({ m_syncObjects[m_swapchainIndex].fence.get() }),
                  LLAMA_DEBUG_INFO, "vk::Device::resetFences() has failed!");

    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    assert_vulkan(m_context->m_device->getGraphicsQueue().queueHandle.submit({ vk::SubmitInfo(1, &m_syncObjects[nextIndex].renderSemaphore.get(), &flags,
                                                                                              1, &m_commandBuffers[m_swapchainIndex].get(),
                                                                                              1, &m_syncObjects[m_swapchainIndex].presentSemaphore.get()) }, 
                                                                             m_syncObjects[m_swapchainIndex].fence.get()),
                  LLAMA_DEBUG_INFO, "vk::Queue::submit() has failed!");

    vk::Result r = m_context->m_device->getGraphicsQueue().queueHandle.presentKHR(vk::PresentInfoKHR(1, &m_syncObjects[m_swapchainIndex].presentSemaphore.get(),
                                                                                                     1, &m_context->m_swapchain.get(), &m_swapchainIndex));
    if (r == vk::Result::eSuccess)
        return;

    if (r == vk::Result::eSuboptimalKHR || r == vk::Result::eErrorOutOfDateKHR)
    {
        assert_vulkan(m_context->getDevice().waitIdle(),
                      LLAMA_DEBUG_INFO, "vk::Device::waitIdle() failed!");

        m_context->recreate();
        recordCommandBuffers();
    }
    else
        assert_vulkan(result.result, LLAMA_DEBUG_INFO, "vk::Device::acquireNextImageKHR() failed!");
}

void llama::Renderer_IVulkan::setShader(Shader shader)
{
    t_shader = std::static_pointer_cast<Shader_IVulkan>(shader);
    recordCommandBuffers();
}

void llama::Renderer_IVulkan::recordCommandBuffers()
{
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

        m_commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, t_shader->getPipeline());
        m_commandBuffers[i]->draw(3, 1, 0, 0);

        m_commandBuffers[i]->endRenderPass();
        m_commandBuffers[i]->end();

        logfile()->print(Colors::WHITE, "Updated CommandBuffer %p", m_commandBuffers[i].get());
    }
}
