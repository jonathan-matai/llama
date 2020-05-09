#include "llpch.h"
#include "graphics/llimage.h"

#include "vulkan/llimage_vk.h"

llama::SampledImage llama::createSampledImage(GraphicsDevice device, std::string_view path)
{
    return std::make_shared<SampledImage_IVulkan>(std::static_pointer_cast<GraphicsDevice_IVulkan>(device), path);
}
