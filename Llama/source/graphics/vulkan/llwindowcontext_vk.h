#pragma once

#include "graphics/llwindowcontext.h"
#include "graphics/llwindow.h"


#include "llimage_vk.h"
#include "llgraphics_vk.h"

namespace llama
{
    class WindowContext_IVulkan : public WindowContext_T
    {
        friend class Renderer_IVulkan;

    public:

        WindowContext_IVulkan(Window window, GraphicsDevice device);
        ~WindowContext_IVulkan() override;

        vk::Device getDevice() const { return m_device->getDevice(); }
        vk::RenderPass getRenderPass() const { return m_renderPass.get(); }

    private:

        bool createVulkanSurface();
        bool createSwapchain();
        bool createRenderPass();
        bool createFramebuffers();

        vk::SurfaceFormatKHR pickFormat();
        vk::PresentModeKHR pickPresentMode();

        std::shared_ptr<GraphicsDevice_IVulkan> m_device;

        vk::UniqueSurfaceKHR m_surface;
        vk::UniqueSwapchainKHR m_swapchain;
        uint32_t m_swapchainWidth, m_swapchainHeight;
        std::vector<vk::UniqueImageView> m_swapchainImageViews;
        vk::Format m_swapchainFormat;
        vk::UniqueRenderPass m_renderPass;
        std::vector<vk::UniqueFramebuffer> m_frameBuffers;

        Window m_window;

        std::unique_ptr<llama::DepthImage_Vulkan> m_depthImage;
        std::unique_ptr<llama::ColorImage_Vulkan> m_colorImage;

    };
}