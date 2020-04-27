#pragma once

#include "llgraphics_vk.h"


namespace llama
{
    class Image_Vulkan
    {
    public:

        Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, 
                     uint32_t width, 
                     uint32_t height, 
                     vk::Format format, 
                     vk::ImageUsageFlags usage, 
                     vma::MemoryUsage memoryType,
                     vk::SampleCountFlagBits msaa = vk::SampleCountFlagBits::e1);
        virtual ~Image_Vulkan();

        static vk::Format findFormat(vk::PhysicalDevice physicalDevice, std::initializer_list<vk::Format> options, vk::ImageTiling tiling, vk::FormatFeatureFlags featues);

        inline vk::Image getImage() const { return m_image.first; }

    protected:

        std::shared_ptr<GraphicsDevice_IVulkan> m_device;
        std::pair<vk::Image, vma::Allocation> m_image;
        vk::Format m_format;

    };

    class DepthImage_Vulkan : public Image_Vulkan
    {
    public:
        
        DepthImage_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device,
                          uint32_t width, uint32_t height,
                          vk::SampleCountFlagBits msaaLevel);
    };

    class ColorImage_Vulkan : public Image_Vulkan
    {
    public:

        ColorImage_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device,
                          uint32_t width, uint32_t height,
                          vk::Format format,
                          vk::SampleCountFlagBits msaaLevel);
    };
}