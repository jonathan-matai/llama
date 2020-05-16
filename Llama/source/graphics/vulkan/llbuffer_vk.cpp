#include "llpch.h"
#include "llbuffer_vk.h"

llama::Buffer_Vulkan::Buffer_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, size_t size, const void* data, vk::BufferUsageFlags usage, bool mapped) :
    m_allocator(device->getAllocator()),
    m_mappedData(nullptr)
{
    if (mapped)
    {
        vma::AllocationInfo allocationInfo;

        if (!assert_vulkan(m_allocator.createBuffer(vk::BufferCreateInfo({},
                                                                         size,
                                                                         usage),
                                                    vma::AllocationCreateInfo(vma::AllocationCreateFlagBits::eMapped,
                                                                              vma::MemoryUsage::eCpuToGpu),
                                                    allocationInfo),
                           m_buffer, LLAMA_DEBUG_INFO, "vma::Allocator::createBuffer() failed!"))
            throw std::runtime_error("Creating buffer failed!");

        if(data)
            memcpy(allocationInfo.pMappedData, data, size);

        m_mappedData = allocationInfo.pMappedData;
    }
    else
    {
        std::pair<vk::Buffer, vma::Allocation> stagingBuffer;
        vma::AllocationInfo allocationInfo;


        if (!assert_vulkan(m_allocator.createBuffer(vk::BufferCreateInfo({},
                                                                         size,
                                                                         vk::BufferUsageFlagBits::eTransferSrc),
                                                    vma::AllocationCreateInfo(vma::AllocationCreateFlagBits::eMapped,
                                                                              vma::MemoryUsage::eCpuToGpu),
                                                    allocationInfo),
                           stagingBuffer, LLAMA_DEBUG_INFO, "vma::Allocator::createBuffer() failed!"))
            throw std::runtime_error("Creating buffer failed!");

        if(data)
            memcpy(allocationInfo.pMappedData, data, size);

        if (!assert_vulkan(m_allocator.createBuffer(vk::BufferCreateInfo({},
                                                                         size,
                                                                         usage | vk::BufferUsageFlagBits::eTransferDst),
                                                    vma::AllocationCreateInfo({},
                                                                              vma::MemoryUsage::eGpuOnly)),
                           m_buffer, LLAMA_DEBUG_INFO, "vma::Allocator::createBuffer() failed!"))
            throw std::runtime_error("Creating buffer failed!");

        device->executeOnDevice([stagingBuffer, size, this](vk::CommandBuffer buffer)
        {
            vk::BufferCopy region(0, 0, size);
            buffer.copyBuffer(stagingBuffer.first, m_buffer.first, 1, &region);

        }, device->getTransferQueue());

        device->getAllocator().destroyBuffer(stagingBuffer.first, stagingBuffer.second);
    }
}

llama::Buffer_Vulkan::~Buffer_Vulkan()
{
    m_allocator.destroyBuffer(m_buffer.first, m_buffer.second);
}

void* llama::ConstantBuffer_IVulkan::at(uint32_t element, bool forceIndex)
{
    // Cast to byte* to increment pointer
    return static_cast<byte*>(m_mappedData) + offset(element, forceIndex ? 0 : m_renderer->getSwapchainIndex());
}

size_t llama::ConstantBuffer_IVulkan::offset(uint32_t element, uint32_t swapchainIndex)
{
    if (m_swapchainSize == 1)
        return m_alignedSize * element;

    return m_alignedSize * (element + swapchainIndex * m_elementCount);
}

size_t llama::ConstantBuffer_IVulkan::size()
{
    return m_alignedSize;
}
