#pragma once

#include "graphics/llbuffer.h"

#include "llgraphics_vk.h"

namespace llama
{
    class Buffer_Vulkan
    {

    public:

        Buffer_Vulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, size_t size, const void* data, vk::BufferUsageFlags usage, bool mapped = false);

        virtual ~Buffer_Vulkan();

        vk::Buffer getBuffer() const { return m_buffer.first; }

        size_t getAlignedSize(size_t size, size_t alignment)
        {
            return ((size + alignment - 1) / alignment) * alignment;
        }

    protected:

        vma::Allocator m_allocator;
        std::pair<vk::Buffer, vma::Allocation> m_buffer;
        void* m_mappedData;
    };

    class VertexBuffer_IVulkan : public VertexBuffer_T, public Buffer_Vulkan
    {
    public:

        VertexBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, size_t size, const void* data) :
            Buffer_Vulkan(device, size, data, vk::BufferUsageFlagBits::eVertexBuffer, false)
        { }
    };

    class IndexBuffer_IVulkan : public IndexBuffer_T, public Buffer_Vulkan
    {
    public:

        IndexBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, const std::vector<uint16_t>& indices) :
            Buffer_Vulkan(device, indices.size() * sizeof(uint16_t), indices.data(), vk::BufferUsageFlagBits::eIndexBuffer, false)
        { }

        IndexBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, const std::vector<uint32_t>& indices) :
            Buffer_Vulkan(device, indices.size() * sizeof(uint32_t), indices.data(), vk::BufferUsageFlagBits::eIndexBuffer, false)
        { }


    };

    class ConstantBuffer_IVulkan : public ConstantBuffer_T, public Buffer_Vulkan
    {
        friend class ConstantSet_IVulkan;

    public:

        ConstantBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, size_t elementSize, uint32_t elementCount, uint32_t swapchainSize) :
            Buffer_Vulkan(device, getAlignedSize(elementSize, device->getConstantBufferAlignment()) * elementCount * swapchainSize, nullptr, vk::BufferUsageFlagBits::eUniformBuffer, true),
            m_elementCount(elementCount),
            m_swapchainSize(swapchainSize),
            m_alignedSize(getAlignedSize(elementSize, device->getConstantBufferAlignment()))
        { }

        void* at(uint32_t element, uint32_t swapchainIndex) override;
        size_t offset(uint32_t element, uint32_t swapchainIndex = 0);
        size_t size();
    private:

        size_t m_alignedSize;
        uint32_t m_elementCount;
        uint32_t m_swapchainSize;
    };
}