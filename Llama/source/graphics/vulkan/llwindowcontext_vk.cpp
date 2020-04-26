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

    return false;
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
