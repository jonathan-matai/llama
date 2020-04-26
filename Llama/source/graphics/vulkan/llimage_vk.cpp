#include "llcore.h"

/*

#include "llimage_vk.h"

llama::Image_Vulkan::Image_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryFlags)
{


    //vmaCreateImage()
    
    assert_vulkan(device->getDevice().createImageUnique(vk::ImageCreateInfo({}, // Flags
                                                                            vk::ImageType::e2D, // Image Type
                                                                            format, // Format
                                                                            { width, height, 1 }, // Extent
                                                                            1, 1, // Mip Levels, Array Layers
                                                                            vk::SampleCountFlagBits::e1,
                                                                            vk::ImageTiling::eOptimal,
                                                                            vk::ImageUsageFlagBits::eColorAttachment,
                                                                            vk::SharingMode::eExclusive, 0, nullptr,
                                                                            vk::ImageLayout::eUndefined)), m_image, LLAMA_DEBUG_INFO, "vk::Device::createImageUnique() failed!");
                                                                            
}

llama::DepthImage_Vulkan::DepthImage_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, uint32_t width, uint32_t height, vk::SampleCountFlagBits msaaLevel)
{
}
*/