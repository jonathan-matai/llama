#include "llpch.h"
#include "llrenderer_vk.h"

llama::Renderer_IVulkan::Renderer_IVulkan(std::shared_ptr<WindowContext_IVulkan> context, std::shared_ptr<Shader_IVulkan> shader) :
    m_context(context),
    m_swapchainIndex(1)
{
    m_syncObjects.resize(context->m_frameBuffers.size());

    for (auto& a : m_syncObjects)
    {
        assert_vulkan(context->getDevice().createSemaphoreUnique(vk::SemaphoreCreateInfo()),
                      a.renderSemaphore, LLAMA_DEBUG_INFO, "vk::Device::createSemaphoreUnique() failed!");

        assert_vulkan(context->getDevice().createSemaphoreUnique(vk::SemaphoreCreateInfo()),
                      a.presentSemaphore, LLAMA_DEBUG_INFO, "vk::Device::createSemaphoreUnique() failed!");

        assert_vulkan(context->getDevice().createFenceUnique(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled)),
                      a.fence, LLAMA_DEBUG_INFO, "vk::Device::createFenceUnique() failed!");
    }

    std::lock_guard lock(context->m_device->getGraphicsQueue().queueFamily->m_commandPoolMutex);

    vk::CommandPool pool = context->m_device->getGraphicsQueue().queueFamily->m_commandPool.get();

    assert_vulkan(context->getDevice().allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(pool, 
                                                                                                  vk::CommandBufferLevel::ePrimary, 
                                                                                                  static_cast<uint32_t>(m_syncObjects.size()))),
                  m_commandBuffers, LLAMA_DEBUG_INFO, "vk::Device::allocateCommandBuffersUnique() failed!");

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

        vk::RenderPassBeginInfo beginInfo(context->getRenderPass(),
                                          context->m_frameBuffers[i].get(),
                                          vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(context->m_swapchainWidth, context->m_swapchainHeight)),
                                          static_cast<uint32_t>(clearValues.size()), clearValues.data());

        m_commandBuffers[i]->beginRenderPass(&beginInfo, vk::SubpassContents::eInline);

        m_commandBuffers[i]->setViewport(0, { vk::Viewport(0.0f, // xOffset
                                                           0.0f, // yOfffset
                                                           static_cast<float>(context->m_swapchainWidth), // Width
                                                           static_cast<float>(context->m_swapchainHeight), // Height
                                                           0.0f,
                                                           1.0f) });

        m_commandBuffers[i]->setScissor(0, { beginInfo.renderArea });

        m_commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, shader->getPipeline());
        m_commandBuffers[i]->draw(3, 1, 0, 0);

        m_commandBuffers[i]->endRenderPass();
        m_commandBuffers[i]->end();

        logfile()->print(Colors::WHITE, "Updated CommandBuffer %p", m_commandBuffers[i].get());
    }
}

llama::Renderer_IVulkan::~Renderer_IVulkan()
{
    m_context->getDevice().waitIdle();
}

void llama::Renderer_IVulkan::tick()
{

    uint32_t nextIndex;

    assert_vulkan(m_context->getDevice().acquireNextImageKHR(m_context->m_swapchain.get(), UINT64_MAX, m_syncObjects[m_swapchainIndex].renderSemaphore.get(), nullptr),
                  nextIndex, LLAMA_DEBUG_INFO, "vk::Device::acquireNextImageKHR() has failed!");

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

    assert_vulkan(m_context->m_device->getGraphicsQueue().queueHandle.presentKHR(vk::PresentInfoKHR(1, &m_syncObjects[m_swapchainIndex].presentSemaphore.get(),
                                                                                                    1, &m_context->m_swapchain.get(), &m_swapchainIndex)),
                  LLAMA_DEBUG_INFO, "vk::Queue::presentKHR() has failed!");
}
