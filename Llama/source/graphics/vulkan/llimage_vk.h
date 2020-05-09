#pragma once


#include "llgraphics_vk.h"
#include "graphics/llimage.h"

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
                     vk::ImageAspectFlags aspect,
                     vma::MemoryUsage memoryType,
                     vk::SampleCountFlagBits msaa = vk::SampleCountFlagBits::e1);

        Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device,
                     std::string_view path,
                     vk::ImageUsageFlags usage,
                     vk::ImageAspectFlags aspect,
                     vma::MemoryUsage memoryType);

        virtual ~Image_Vulkan();

        static vk::Format findFormat(vk::PhysicalDevice physicalDevice, std::initializer_list<vk::Format> options, vk::ImageTiling tiling, vk::FormatFeatureFlags featues);

        inline vk::Image getImage() const { return m_image.first; }
        inline vk::ImageView getImageView() const { return m_imageView;  }
        inline vk::Format getFormat() const { return m_format; }
        inline vk::SampleCountFlagBits getMsaa() const { return m_msaa; }

    protected:

        std::shared_ptr<GraphicsDevice_IVulkan> m_device;
        std::pair<vk::Image, vma::Allocation> m_image;
        vk::ImageView m_imageView;
        vk::Format m_format;
        vk::SampleCountFlagBits m_msaa;

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

    class SampledImage_IVulkan : public SampledImage_T, public Image_Vulkan
    {
        friend class ConstantSet_IVulkan;

    public:

        SampledImage_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, std::string_view path);

    private:

        vk::UniqueSampler m_sampler;
    };
}