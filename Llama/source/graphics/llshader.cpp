#include "llpch.h"
#include "graphics/llshader.h"

#include "vulkan/llshader_vk.h"

llama::Shader llama::createShader(WindowContext context, std::string_view vertexShaderSpv, std::string_view fragmentShaderSpv)
{
    return std::make_shared<Shader_IVulkan>(std::static_pointer_cast<WindowContext_IVulkan>(context), vertexShaderSpv, fragmentShaderSpv);
}
