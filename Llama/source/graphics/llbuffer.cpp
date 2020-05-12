#include "llpch.h"
#include "graphics/llbuffer.h"

#include "vulkan/llbuffer_vk.h"

llama::VertexBuffer llama::createVertexBuffer(GraphicsDevice device, size_t size, const void* data)
{
    return std::make_shared<VertexBuffer_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), size, data);
}

llama::IndexBuffer llama::createIndexBuffer(GraphicsDevice device, const std::vector<uint16_t>& indices)
{
    return std::make_shared<IndexBuffer_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), indices);
}

llama::IndexBuffer llama::createIndexBuffer(GraphicsDevice device, const std::vector<uint32_t>& indices)
{
    return std::make_shared<IndexBuffer_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), indices);
}

llama::ConstantBuffer llama::createConstantBuffer(GraphicsDevice device, size_t elementSize, uint32_t elementCount, uint32_t swapchainSize)
{
    return std::make_shared<ConstantBuffer_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), elementSize, elementCount, swapchainSize);
}

namespace llama
{
    class ConstantArrayBuffer_I : public ConstantArrayBuffer_T
    {
    public:

        ConstantArrayBuffer_I(GraphicsDevice device, size_t elementSize, uint32_t maxElementCount, uint32_t swapchainSize);

        uint32_t addElement() override;
        void removeElement(uint32_t index) override;

        void* at(uint32_t element, uint32_t swapchainIndex) override { return m_buffer->at(element, swapchainIndex); };

    private:

        ConstantBuffer_T* getBuffer() const override { return m_buffer.get(); }

        std::unique_ptr<ConstantBuffer_T> m_buffer;
        uint32_t m_head;
    };
}

llama::ConstantArrayBuffer llama::createConstantArrayBuffer(GraphicsDevice device, size_t elementSize, uint32_t maxElementCount, uint32_t swapchainSize)
{
    return std::make_shared<ConstantArrayBuffer_I>(device, elementSize, maxElementCount, swapchainSize);
}

llama::ConstantArrayBuffer_I::ConstantArrayBuffer_I(GraphicsDevice device, size_t elementSize, uint32_t maxElementCount, uint32_t swapchainSize) :
    m_buffer(std::make_unique<ConstantBuffer_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), elementSize, maxElementCount, swapchainSize)),
    m_head(0)
{
    for (uint32_t i = 0; i < maxElementCount - 1; ++i)
        *reinterpret_cast<uint32_t*>(m_buffer->at(i, 0)) = i + 1;
    *reinterpret_cast<uint32_t*>(m_buffer->at(maxElementCount - 1, 0)) = UINT32_MAX;
}

uint32_t llama::ConstantArrayBuffer_I::addElement()
{
    uint32_t head = m_head;

    m_head = *reinterpret_cast<uint32_t*>(m_buffer->at(head, 0));

    if (m_head == UINT32_MAX)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "ConstantArrayBuffer %p overflowed!", this);
        throw std::runtime_error("ConstantArrayBuffer overflowed!");
    }

    return head;
}

void llama::ConstantArrayBuffer_I::removeElement(uint32_t index)
{
    *reinterpret_cast<uint32_t*>(m_buffer->at(index, 0)) = m_head;
    m_head = index;
}
