#include "llcore.h"
#include "graphics/llwindowcontext.h"

#include "vulkan/llwindowcontext_vk.h"

llama::WindowContext llama::createWindowContext(Window window, GraphicsDevice device)
{
    return std::make_shared<WindowContext_IVulkan>(window, device);
}
