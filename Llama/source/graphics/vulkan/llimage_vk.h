#pragma once

#include "llgraphics_vk.h"


namespace llama
{
    class Image_Vulkan
    {
    public:

        Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, 
                     uint32_t width, uint32_t height, 
                     vk::Format format, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryFlags);
        virtual ~Image_Vulkan() { }

        inline vk::Image getImage() const { return m_image.get(); }

    private:

        vk::UniqueImage m_image;

    };

    class DepthImage_Vulkan : public Image_Vulkan
    {
    public:
        
        DepthImage_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device,
                          uint32_t width, uint32_t height,
                          vk::SampleCountFlagBits msaaLevel);
    };
}