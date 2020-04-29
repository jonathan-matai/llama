#include "llpch.h"
#include "graphics/llshader.h"

#include "vulkan/llshader_vk.h"

llama::Shader llama::createShader(Renderer renderer, std::string_view vertexShaderSpv, std::string_view fragmentShaderSpv)
{
    return std::make_shared<Shader_IVulkan>(std::static_pointer_cast<Renderer_IVulkan>(renderer), vertexShaderSpv, fragmentShaderSpv);
}
