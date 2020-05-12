#include "llpch.h"
#include "graphics/llrenderer.h"

#include "graphics/llshader.h"

#include "vulkan/llrenderer_vk.h"

llama::Renderer llama::createRenderer(EventNode node, GraphicsDevice device, Window window)
{
    return std::make_shared<Renderer_IVulkan>(node, std::static_pointer_cast<GraphicsDevice_IVulkan>(device), window);
}
