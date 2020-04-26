#pragma once

#include "graphics/llwindowcontext.h"
#include "graphics/llwindow.h"
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

        vk::UniqueSurfaceKHR m_surface;
        vk::UniqueSwapchainKHR m_swapchain;

        Window m_window;
        std::shared_ptr<GraphicsDevice_IVulkan> m_device;
    };
}