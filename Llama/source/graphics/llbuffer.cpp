#include "llpch.h"
#include "graphics/llbuffer.h"

#include "vulkan/llrenderer_vk.h"
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

llama::ConstantBuffer llama::createConstantBuffer(Renderer renderer, size_t elementSize, uint32_t elementCount)
{
    return std::make_shared<ConstantBuffer_IVulkan>(std::static_pointer_cast<Renderer_IVulkan>(renderer), elementSize, elementCount);
}

namespace llama
{
    class ConstantArrayBuffer_I : public ConstantArrayBuffer_T
    {
    public:

        ConstantArrayBuffer_I(Renderer renderer, size_t elementSize, uint32_t maxElementCount);

        uint32_t addElement() override;
        void removeElement(uint32_t index) override;

        void* at(uint32_t element) override { return m_buffer->at(element); };

    private:

        ConstantBuffer_T* getBuffer() const override { return m_buffer.get(); }

        std::unique_ptr<ConstantBuffer_T> m_buffer;
        uint32_t m_head;
    };
}

llama::ConstantArrayBuffer llama::createConstantArrayBuffer(Renderer renderer, size_t elementSize, uint32_t maxElementCount)
{
    return std::make_shared<ConstantArrayBuffer_I>(renderer, elementSize, maxElementCount);
}

llama::ConstantArrayBuffer_I::ConstantArrayBuffer_I(Renderer renderer, size_t elementSize, uint32_t maxElementCount) :
    m_buffer(std::make_unique<ConstantBuffer_IVulkan>(std::static_pointer_cast<Renderer_IVulkan>(renderer), elementSize, maxElementCount)),
    m_head(0)
{
    for (uint32_t i = 0; i < maxElementCount - 1; ++i)
        *reinterpret_cast<uint32_t*>(m_buffer->at(i, true)) = i + 1;
    *reinterpret_cast<uint32_t*>(m_buffer->at(maxElementCount - 1, true)) = UINT32_MAX;
}

uint32_t llama::ConstantArrayBuffer_I::addElement()
{
    uint32_t head = m_head;

    m_head = *reinterpret_cast<uint32_t*>(m_buffer->at(head, true));

    if (m_head == UINT32_MAX)
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "ConstantArrayBuffer %p overflowed!", this);
        throw std::runtime_error("ConstantArrayBuffer overflowed!");
    }

    return head;
}

void llama::ConstantArrayBuffer_I::removeElement(uint32_t index)
{
    *reinterpret_cast<uint32_t*>(m_buffer->at(index, true)) = m_head;
    m_head = index;
}
