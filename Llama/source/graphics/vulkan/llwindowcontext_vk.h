#pragma once

#include "graphics/llwindowcontext.h"
#include "graphics/llwindow.h"


#include "llimage_vk.h"
#include "llgraphics_vk.h"

namespace llama
{
    class WindowContext_IVulkan : public WindowContext_T
    {
    public:

        WindowContext_IVulkan(Window window, GraphicsDevice device);
        ~WindowContext_IVulkan() override;

    private:

        bool createVulkanSurface();
        bool createSwapchain();

        vk::SurfaceFormatKHR pickFormat();
        vk::PresentModeKHR pickPresentMode();

        std::shared_ptr<GraphicsDevice_IVulkan> m_device;

        vk::UniqueSurfaceKHR m_surface;
        vk::UniqueSwapchainKHR m_swapchain;
        uint32_t m_swapchainWidth, m_swapchainHeight;
        std::vector<vk::Image> m_swapchainImages;
        std::vector<vk::UniqueImageView> m_swapchainImageViews;
        vk::Format m_swapchainFormat;

        Window m_window;

        std::unique_ptr<llama::DepthImage_Vulkan> m_depthImage;
        std::unique_ptr<llama::ColorImage_Vulkan> m_colorImage;

    };
}