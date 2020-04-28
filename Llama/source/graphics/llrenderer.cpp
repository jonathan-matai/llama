#include "llcore.h"
#include "graphics/llrenderer.h"

#include "vulkan/llrenderer_vk.h"

llama::Renderer llama::createRenderer(WindowContext context, Shader shader)
{
    return std::make_shared<Renderer_IVulkan>(std::static_pointer_cast<WindowContext_IVulkan>(context), 
                                              std::static_pointer_cast<Shader_IVulkan>(shader));
}
