#pragma once

#include "graphics/llbuffer.h"

#include "llrenderer_vk.h"

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

        VertexBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, size_t vertexSize, size_t vertexCount, const void* data) :
            Buffer_Vulkan(device, vertexSize * vertexCount, data, vk::BufferUsageFlagBits::eVertexBuffer, false),
            m_vertexCount(vertexCount)
        { }

        size_t m_vertexCount;
    };

    class IndexBuffer_IVulkan : public IndexBuffer_T, public Buffer_Vulkan
    {
    public:

        IndexBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, const std::vector<uint16_t>& indices) :
            Buffer_Vulkan(device, indices.size() * sizeof(uint16_t), indices.data(), vk::BufferUsageFlagBits::eIndexBuffer, false),
            m_32bitIndices(false),
            m_indexCount(indices.size())
        { }

        IndexBuffer_IVulkan(std::shared_ptr<GraphicsDevice_IVulkan> device, const std::vector<uint32_t>& indices) :
            Buffer_Vulkan(device, indices.size() * sizeof(uint32_t), indices.data(), vk::BufferUsageFlagBits::eIndexBuffer, false),
            m_32bitIndices(true),
            m_indexCount(indices.size())
        { }

        bool m_32bitIndices;
        size_t m_indexCount;
    };

    class ConstantBuffer_IVulkan : public ConstantBuffer_T, public Buffer_Vulkan
    {
        friend class ConstantSet_IVulkan;

    public:

        ConstantBuffer_IVulkan(std::shared_ptr<Renderer_IVulkan> renderer, size_t elementSize, uint32_t elementCount) :
            Buffer_Vulkan(renderer->getGraphicsDevice(), 
                          getAlignedSize(elementSize, renderer->getGraphicsDevice()->getConstantBufferAlignment()) * elementCount * renderer->getSwapchainSize(), 
                          nullptr, 
                          vk::BufferUsageFlagBits::eUniformBuffer, 
                          true),
            m_elementCount(elementCount),
            m_swapchainSize(renderer->getSwapchainSize()),
            m_alignedSize(getAlignedSize(elementSize, renderer->getGraphicsDevice()->getConstantBufferAlignment())),
            m_renderer(renderer)
        { }

        void* at(uint32_t element, bool forceIndex = false) override;
        size_t offset(uint32_t element, uint32_t swapchainIndex);
        size_t size();
    private:

        std::shared_ptr<Renderer_IVulkan> m_renderer;
        size_t m_alignedSize;
        uint32_t m_elementCount;
        uint32_t m_swapchainSize;
    };
}