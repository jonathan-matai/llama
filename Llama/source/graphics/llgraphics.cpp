#include "llpch.h"
#include "graphics/llgraphics.h"

#include "vulkan/llgraphics_vk.h"

llama::GraphicsDevice llama::createGraphicsDevice()
{
    return std::make_shared<GraphicsDevice_IVulkan>();
}
