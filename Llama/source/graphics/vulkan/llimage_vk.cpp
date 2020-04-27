#include "llcore.h"

#include "llimage_vk.h"

llama::Image_Vulkan::Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, 
                                  uint32_t width, 
                                  uint32_t height, 
                                  vk::Format format, 
                                  vk::ImageUsageFlags usage, 
                                  vma::MemoryUsage memoryType,
                                  vk::SampleCountFlagBits msaa) :
    m_device(device),
    m_format(format)
{

    assert_vulkan(device->getAllocator().createImage(vk::ImageCreateInfo({}, // Flags
                                                                         vk::ImageType::e2D, // Image Type
                                                                         format, // Format
                                                                         { width, height, 1 }, // Extent
                                                                         1, 1, // Mip Levels, Array Layers
                                                                         msaa,
                                                                         vk::ImageTiling::eOptimal,
                                                                         usage,
                                                                         vk::SharingMode::eExclusive, 0, nullptr,
                                                                         vk::ImageLayout::eUndefined),
                                                     vma::AllocationCreateInfo({}, // Flags
                                                                               memoryType)), 
                  m_image, LLAMA_DEBUG_INFO, "vma::Allocator::createImage() failed!");
                                                                            
}

llama::Image_Vulkan::~Image_Vulkan()
{
    m_device->getAllocator().destroyImage(m_image.first, m_image.second);
}

vk::Format llama::Image_Vulkan::findFormat(vk::PhysicalDevice physicalDevice, std::initializer_list<vk::Format> options, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (auto a : options)
    {
        vk::FormatProperties properties = physicalDevice.getFormatProperties(a);

        if ((tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features)) ||
            (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features)))
            return a;
    }

    logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "GPU doesnt' support a format that is required in this engine!");
    return vk::Format::eUndefined;
}

llama::DepthImage_Vulkan::DepthImage_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, uint32_t width, uint32_t height, vk::SampleCountFlagBits msaaLevel) :
    Image_Vulkan(device, 
                 width, 
                 height, 
                 findFormat(device->getPhysicalDevice(), 
                            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint }, 
                            vk::ImageTiling::eOptimal, 
                            vk::FormatFeatureFlagBits::eDepthStencilAttachment), 
                 vk::ImageUsageFlagBits::eDepthStencilAttachment, 
                 vma::MemoryUsage::eGpuOnly)
{

    device->executeOnDevice([this](vk::CommandBuffer buffer)
    {

        buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, // Source Stage Mask
                               vk::PipelineStageFlagBits::eEarlyFragmentTests, // Destination Stage Mask
                               {}, // Dependancy Flags
                               {}, // Memory Barrier
                               {}, // Buffer Memory Barrier
                               { 
                                   vk::ImageMemoryBarrier({}, // Source Access Mask
                                                          vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite, // Destination Access Mask
                                                          vk::ImageLayout::eUndefined, // Old Layout
                                                          vk::ImageLayout::eDepthStencilAttachmentOptimal, // New Layout
                                                          VK_QUEUE_FAMILY_IGNORED, // Source Queue Family Index
                                                          VK_QUEUE_FAMILY_IGNORED, // Destination Queue Family Index
                                                          m_image.first, // Image
                                                          vk::ImageSubresourceRange(m_format == vk::Format::eD32Sfloat ? 
                                                                                        vk::ImageAspectFlagBits::eDepth : (vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil),
                                                                                    0, 1, // Mip Level and Level Count
                                                                                    0, 1 /* Array Layer and Layer Count */))
                               } /* Image Memory Barrier */);

    }, device->getGraphicsQueue());
}

llama::ColorImage_Vulkan::ColorImage_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, uint32_t width, uint32_t height, vk::Format format, vk::SampleCountFlagBits msaaLevel) :
    Image_Vulkan(device,
                 width,
                 height,
                 format,
                 vk::ImageUsageFlagBits::eColorAttachment,
                 vma::MemoryUsage::eGpuOnly)
{

    device->executeOnDevice([this](vk::CommandBuffer buffer)
    {

        buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, // Source Stage Mask
                               vk::PipelineStageFlagBits::eColorAttachmentOutput, // Destination Stage Mask
                               {}, // Dependancy Flags
                               {}, // Memory Barrier
                               {}, // Buffer Memory Barrier
                               {
                                   vk::ImageMemoryBarrier({}, // Source Access Mask
                                                          vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, // Destination Access Mask
                                                          vk::ImageLayout::eUndefined, // Old Layout
                                                          vk::ImageLayout::eColorAttachmentOptimal, // New Layout
                                                          VK_QUEUE_FAMILY_IGNORED, // Source Queue Family Index
                                                          VK_QUEUE_FAMILY_IGNORED, // Destination Queue Family Index
                                                          m_image.first, // Image
                                                          vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, // Image Aspect Flags
                                                                                    0, 1, // Mip Level and Level Count
                                                                                    0, 1 /* Array Layer and Layer Count */))
                               } /* Image Memory Barrier */);

    }, device->getGraphicsQueue());
}