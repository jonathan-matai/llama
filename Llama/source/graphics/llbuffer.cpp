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
