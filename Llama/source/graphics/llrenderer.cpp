#include "llpch.h"
#include "graphics/llrenderer.h"

#include "graphics/llshader.h"

#include "vulkan/llrenderer_vk.h"

llama::Renderer llama::createRenderer(GraphicsDevice device, Window window)
{
    return std::make_shared<Renderer_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), window);
}
