#include "llpch.h"

#include "llimage_vk.h"

#include <stb_image.h>

llama::Image_Vulkan::Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, 
                                  uint32_t width, 
                                  uint32_t height, 
                                  vk::Format format, 
                                  vk::ImageUsageFlags usage,
                                  vk::ImageAspectFlags aspect,
                                  vma::MemoryUsage memoryType,
                                  vk::SampleCountFlagBits msaa) :
    m_device(device),
    m_format(format),
    m_msaa(msaa)
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
                                                                            
    assert_vulkan(m_device->getDevice().createImageView(vk::ImageViewCreateInfo({}, // Flags
                                                                                m_image.first, // Image
                                                                                vk::ImageViewType::e2D, // View Type
                                                                                format, // Format
                                                                                vk::ComponentMapping(),
                                                                                vk::ImageSubresourceRange(aspect,
                                                                                                          0, 1,  // Mip Level and Level Count
                                                                                                          0, 1 /* Array Layer and Layer Count */))), 
                  m_imageView, LLAMA_DEBUG_INFO, "vk::Device::createImageViewUnique() failed!");
}

llama::Image_Vulkan::Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, 
                                  std::string_view path, 
                                  vk::ImageUsageFlags usage, 
                                  vk::ImageAspectFlags aspect, 
                                  vma::MemoryUsage memoryType) :
    m_device(device),
    m_format(vk::Format::eUndefined),
    m_msaa(vk::SampleCountFlagBits::e1)
{

    int x = 0, y = 0, n = 0;

    FILE* image = fopen(std::string(path).c_str(), "rb");

    if (image == nullptr)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Couldn't load Image \"%*s\" from file!", path.length(), path.data());
        throw std::runtime_error("Loading image failed!");
    }

    if (stbi_info_from_file(image, &x, &y, &n) != 1)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "\"%*s\" is not a valid Image!", path.length(), path.data());
        throw std::runtime_error("Loading image failed!");
    }

    fseek(image, 0, SEEK_SET);

    byte* data = stbi_load_from_file(image, &x, &y, &n, n == STBI_rgb ? STBI_rgb_alpha : n);

    if (data == nullptr)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "\"%*s\" is not a valid Image!", path.length(), path.data());
        throw std::runtime_error("Loading image failed!");
    }

    size_t dataSize = x * y * n;

    std::pair<vk::Buffer, vma::Allocation> stagingBuffer;
    vma::AllocationInfo allocationInfo;
    assert_vulkan(m_device->getAllocator().createBuffer(vk::BufferCreateInfo({},
                                                                             dataSize,
                                                                             vk::BufferUsageFlagBits::eTransferSrc), 
                                                        vma::AllocationCreateInfo(vma::AllocationCreateFlagBits::eMapped,
                                                                                  vma::MemoryUsage::eCpuToGpu), 
                                                        &allocationInfo),
                  stagingBuffer, LLAMA_DEBUG_INFO, "vma::Allocator::createBuffer() failed!");

    memcpy(allocationInfo.pMappedData, data, dataSize);
    stbi_image_free(data);

    switch (n)
    {
    case STBI_grey: 
        m_format = vk::Format::eR8Unorm; break;
    case STBI_grey_alpha:
        m_format = vk::Format::eR8G8Unorm; break;
    case STBI_rgb: case STBI_rgb_alpha: 
        m_format = vk::Format::eR8G8B8A8Unorm; break;
    default:
        throw std::runtime_error("Invalid image!");
    }

    assert_vulkan(device->getAllocator().createImage(vk::ImageCreateInfo({}, // Flags
                                                                         vk::ImageType::e2D, // Image Type
                                                                         m_format, // Format
                                                                         { static_cast<uint32_t>(x), static_cast<uint32_t>(y), 1 }, // Extent
                                                                         1, 1, // Mip Levels, Array Layers
                                                                         vk::SampleCountFlagBits::e1,
                                                                         vk::ImageTiling::eOptimal,
                                                                         usage | vk::ImageUsageFlagBits::eTransferDst,
                                                                         vk::SharingMode::eExclusive, 0, nullptr,
                                                                         vk::ImageLayout::eUndefined),
                                                     vma::AllocationCreateInfo({}, // Flags
                                                                               memoryType)),
                  m_image, LLAMA_DEBUG_INFO, "vma::Allocator::createImage() failed!");

    assert_vulkan(m_device->getDevice().createImageView(vk::ImageViewCreateInfo({}, // Flags
                                                                                m_image.first, // Image
                                                                                vk::ImageViewType::e2D, // View Type
                                                                                m_format, // Format
                                                                                vk::ComponentMapping(),
                                                                                vk::ImageSubresourceRange(aspect,
                                                                                                          0, 1,  // Mip Level and Level Count
                                                                                                          0, 1 /* Array Layer and Layer Count */))),
                  m_imageView, LLAMA_DEBUG_INFO, "vk::Device::createImageViewUnique() failed!");

    m_device->executeOnDevice([this, stagingBuffer, x, y](vk::CommandBuffer buffer)
    {
        buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, // Source Stage Mask
                               vk::PipelineStageFlagBits::eTransfer, // Destination Stage Mask
                               {}, // Dependancy Flags
                               {}, // Memory Barrier
                               {}, // Buffer Memory Barrier
                               {
                                   vk::ImageMemoryBarrier({}, // Source Access Mask
                                                          vk::AccessFlagBits::eTransferWrite, // Destination Access Mask
                                                          vk::ImageLayout::eUndefined, // Old Layout
                                                          vk::ImageLayout::eTransferDstOptimal, // New Layout
                                                          VK_QUEUE_FAMILY_IGNORED, // Source Queue Family Index
                                                          VK_QUEUE_FAMILY_IGNORED, // Destination Queue Family Index
                                                          m_image.first, // Image
                                                          vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, // Image Aspect Flags
                                                                                    0, 1, // Mip Level and Level Count
                                                                                    0, 1 /* Array Layer and Layer Count */))
                               } /* Image Memory Barrier */);

        vk::BufferImageCopy region(0, // Offset
                                   0, // Row Stride
                                   0, // Layer Stride
                                   vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
                                                              0, // Mip Level 
                                                              0, // Base Array Layer
                                                              1 /* Layer Count */),
                                   vk::Offset3D(0, 0, 0),
                                   vk::Extent3D(x, y, 1));

        buffer.copyBufferToImage(stagingBuffer.first, m_image.first, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, // Source Stage Mask
                               vk::PipelineStageFlagBits::eFragmentShader, // Destination Stage Mask
                               {}, // Dependancy Flags
                               {}, // Memory Barrier
                               {}, // Buffer Memory Barrier
                               {
                                   vk::ImageMemoryBarrier(vk::AccessFlagBits::eTransferWrite, // Source Access Mask
                                                          vk::AccessFlagBits::eShaderRead, // Destination Access Mask
                                                          vk::ImageLayout::eTransferDstOptimal, // Old Layout
                                                          vk::ImageLayout::eShaderReadOnlyOptimal, // New Layout
                                                          VK_QUEUE_FAMILY_IGNORED, // Source Queue Family Index
                                                          VK_QUEUE_FAMILY_IGNORED, // Destination Queue Family Index
                                                          m_image.first, // Image
                                                          vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, // Image Aspect Flags
                                                                                    0, 1, // Mip Level and Level Count
                                                                                    0, 1 /* Array Layer and Layer Count */))
                               } /* Image Memory Barrier */);

    }, m_device->getGraphicsQueue());

    m_device->getAllocator().destroyBuffer(stagingBuffer.first, stagingBuffer.second);
}

llama::Image_Vulkan::~Image_Vulkan()
{
    m_device->getDevice().destroyImageView(m_imageView);
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
                 vk::ImageAspectFlagBits::eDepth,
                 vma::MemoryUsage::eGpuOnly,
                 msaaLevel)
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
                 vk::ImageAspectFlagBits::eColor,
                 vma::MemoryUsage::eGpuOnly,
                 msaaLevel)
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

llama::SampledImage_IVulkan::SampledImage_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, std::string_view path) :
    Image_Vulkan(device, path, vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlagBits::eColor, vma::MemoryUsage::eGpuOnly)
{
    assert_vulkan(device->getDevice().createSamplerUnique(vk::SamplerCreateInfo({},
                                                                                vk::Filter::eLinear, // Magnification
                                                                                vk::Filter::eLinear, // Minification
                                                                                vk::SamplerMipmapMode::eLinear,
                                                                                vk::SamplerAddressMode::eRepeat, // u
                                                                                vk::SamplerAddressMode::eRepeat, // v
                                                                                vk::SamplerAddressMode::eRepeat, // w
                                                                                0.0f, // Mip LOD Bias
                                                                                VK_TRUE, // Anisotropy
                                                                                16.0f, // Anisotropy Max Level
                                                                                VK_FALSE, // Compare
                                                                                vk::CompareOp::eNever,
                                                                                0.0f, // Min LOD
                                                                                0.0f, // Max LOD
                                                                                vk::BorderColor::eIntOpaqueWhite,
                                                                                VK_FALSE /* Unnormalized coordiantes */)),
                  m_sampler, LLAMA_DEBUG_INFO, "vk::Device::createSamplerUnique() failed!");
}
