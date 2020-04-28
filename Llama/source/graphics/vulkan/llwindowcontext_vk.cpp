#include "llcore.h"
#include "llwindowcontext_vk.h"

#include <GLFW/glfw3.h>
#include "math/llmath.h"

llama::WindowContext_IVulkan::WindowContext_IVulkan(Window window, GraphicsDevice device) :
    m_window(window),
    m_device(std::static_pointer_cast<GraphicsDevice_IVulkan>(device))
{
    
    createVulkanSurface();
    createSwapchain();

    m_depthImage = std::make_unique<DepthImage_Vulkan>(m_device, m_swapchainWidth, m_swapchainHeight, vk::SampleCountFlagBits::e4);
    m_colorImage = std::make_unique<ColorImage_Vulkan>(m_device, m_swapchainWidth, m_swapchainHeight, m_swapchainFormat, vk::SampleCountFlagBits::e4);

    createRenderPass();
    createFramebuffers();
}

llama::WindowContext_IVulkan::~WindowContext_IVulkan()
{
}

bool llama::WindowContext_IVulkan::createVulkanSurface()
{
    VkSurfaceKHR surface;
    VkResult r = glfwCreateWindowSurface(m_device->getInstance(), m_window->getGLFWWindowHandle(), nullptr, &surface);

    if (r != VK_SUCCESS)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "glfwCreateWindowSurface() failed! Cannot create WindowContext! (%s)", vkResultToString(static_cast<vk::Result>(r)));
        return false;
    }

    m_surface = vk::UniqueSurfaceKHR(static_cast<vk::SurfaceKHR>(surface), vk::ObjectDestroy(m_device->getInstance(), nullptr, VULKAN_HPP_DEFAULT_DISPATCHER));

    return true;
}

bool llama::WindowContext_IVulkan::createSwapchain()
{
    vk::SurfaceCapabilitiesKHR caps;

    if (!assert_vulkan(m_device->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_surface.get()), caps, 
                       LLAMA_DEBUG_INFO, "vk::PhysicalDevice::getSurfaceCapabilitiesKHR() failed! Cannot create WindowContext"))
        return false;

    VkBool32 result;

    if (!assert_vulkan(m_device->getPhysicalDevice().getSurfaceSupportKHR(0, m_surface.get()), result, 
                       LLAMA_DEBUG_INFO, "vk::PhysicalDevice::getSurfaceSupportKHR() failed! Cannot create WindowContext!"))
        return false;

    if (result != VK_TRUE)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Vulkan Window Surface created cannot be used for drawing!");
        return false;
    }

    auto format = pickFormat();

    vk::SwapchainCreateInfoKHR swapchain_ci({},                                                 // flags
                                            m_surface.get(),                                    // Surface
                                            clamp(caps.minImageCount, 3u, caps.maxImageCount),  // min image count
                                            format.format,                                      // format
                                            format.colorSpace,                                  // Color Space
                                            caps.currentExtent,                                 // Extent
                                            1,                                                  // Image Array Layers
                                            vk::ImageUsageFlagBits::eColorAttachment,           // Image Usage
                                            vk::SharingMode::eExclusive, 0, nullptr,            // Sharing
                                            caps.currentTransform,                              // Surface Transform
                                            vk::CompositeAlphaFlagBitsKHR::eOpaque,             // Opaque
                                            pickPresentMode(),                                  // Present Mode
                                            VK_TRUE                                             /* clipped */);

    if (!assert_vulkan(m_device->getDevice().createSwapchainKHRUnique(swapchain_ci), m_swapchain, LLAMA_DEBUG_INFO, "vk::Device::createSwapchainKHRUnique() failed! Cannot create WindowContext!"))
        return false;

    m_swapchainWidth = caps.currentExtent.width;
    m_swapchainHeight = caps.currentExtent.height;
    m_swapchainFormat = format.format;

    std::vector<vk::Image> swapchainImages;

    if (!assert_vulkan(m_device->getDevice().getSwapchainImagesKHR(m_swapchain.get()), swapchainImages, LLAMA_DEBUG_INFO, "vk::Device::getSwapchainImagesKHR() failed!"))
        return false;

    m_swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); ++i)
    {
        if (!assert_vulkan(m_device->getDevice().createImageViewUnique(vk::ImageViewCreateInfo({}, // Flags
                                                                                               swapchainImages[i], // Image
                                                                                               vk::ImageViewType::e2D, // Type
                                                                                               format.format, // Format
                                                                                               vk::ComponentMapping(),
                                                                                               vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
                                                                                                                         0, 1, // Mip Level and Count
                                                                                                                         0, 1 /* Base Array Layer and count */))), 
                           m_swapchainImageViews[i], LLAMA_DEBUG_INFO, "vk::Device::getSwapchainImagesKHR() failed!"))
            return false;
    }

    return true;
}

bool llama::WindowContext_IVulkan::createRenderPass()
{
    std::array<vk::AttachmentDescription, 3> attachments
    {
        vk::AttachmentDescription({},
                                  m_swapchainFormat,
                                  vk::SampleCountFlagBits::e1,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eStore,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::ePresentSrcKHR),
        vk::AttachmentDescription({},
                                  m_colorImage->getFormat(),
                                  m_colorImage->getMsaa(),
                                  vk::AttachmentLoadOp::eClear,
                                  vk::AttachmentStoreOp::eStore,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::eColorAttachmentOptimal),
        vk::AttachmentDescription({},
                                  m_depthImage->getFormat(),
                                  m_depthImage->getMsaa(),
                                  vk::AttachmentLoadOp::eClear,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::eDepthStencilAttachmentOptimal)
    };

    vk::AttachmentReference swapchainAttachment(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference colorAttachment(1, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthAttachment(2, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass({},
                                   vk::PipelineBindPoint::eGraphics,
                                   0, nullptr, // Input Attachments
                                   1, &colorAttachment, &swapchainAttachment, &depthAttachment, // Color and depth attachments
                                   0, nullptr /* Resolve Attachments */);

    vk::SubpassDependency dependany(VK_SUBPASS_EXTERNAL, // Dependancy between External subpass and ...
                                    0, // ... first subpass
                                    vk::PipelineStageFlagBits::eColorAttachmentOutput, // Color Attachment Output must finish
                                    vk::PipelineStageFlagBits::eColorAttachmentOutput, // Color Attachment Output must wait for layout transformation
                                    {}, // Source Access Mask
                                    vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, // Destination Access Mask
                                    {} /* Dependancy Flag */);

    if (!assert_vulkan(m_device->getDevice().createRenderPassUnique(vk::RenderPassCreateInfo({},
                                                                                             static_cast<uint32_t>(attachments.size()), attachments.data(),
                                                                                             1, &subpass,
                                                                                             1, &dependany)),
                       m_renderPass, LLAMA_DEBUG_INFO, "vk::Device::createRenderPassUnique() failed!"))
        return false;

    return true;
}

bool llama::WindowContext_IVulkan::createFramebuffers()
{
    m_frameBuffers.resize(m_swapchainImageViews.size());

    for (size_t i = 0; i < m_swapchainImageViews.size(); ++i)
    {
        std::array<vk::ImageView, 3> imageViews = { m_swapchainImageViews[i].get(), m_colorImage->getImageView(), m_depthImage->getImageView() };

        if (!assert_vulkan(m_device->getDevice().createFramebufferUnique(vk::FramebufferCreateInfo({}, // Flags
                                                                                                   m_renderPass.get(), // Render Pass
                                                                                                   static_cast<uint32_t>(imageViews.size()), imageViews.data(), // Attachments
                                                                                                   m_swapchainWidth, m_swapchainHeight, 1 /* width, height, layers*/)),
                           m_frameBuffers[i], LLAMA_DEBUG_INFO, "vk::Device::createFrameBufferUnique() failed!"))
            return false;
    }

    return true;
}

vk::SurfaceFormatKHR llama::WindowContext_IVulkan::pickFormat()
{
    std::vector<vk::SurfaceFormatKHR> formats;
    assert_vulkan(m_device->getPhysicalDevice().getSurfaceFormatsKHR(m_surface.get()), formats, 
                  LLAMA_DEBUG_INFO, "vk::PhysicalDevice::getSurfaceFormatsKHR() failed! Cannot create WindowContext!");

    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };

    for (const auto& a : formats)
        if (a.format == vk::Format::eB8G8R8A8Unorm && a.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return a;

    return formats[0];
}

vk::PresentModeKHR llama::WindowContext_IVulkan::pickPresentMode()
{
    std::vector<vk::PresentModeKHR> presentModes;
    assert_vulkan(m_device->getPhysicalDevice().getSurfacePresentModesKHR(m_surface.get()), presentModes,
                  LLAMA_DEBUG_INFO, "vk::PhysicalDevice::getSurfacePresentModesKHR() failed! Cannot create WindowContext!");

    for (const auto a : presentModes)
        if (a == vk::PresentModeKHR::eMailbox)
            return vk::PresentModeKHR::eMailbox;

    return vk::PresentModeKHR::eFifo;
}
