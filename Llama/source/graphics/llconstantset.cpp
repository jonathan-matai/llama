#include "llpch.h"
#include "graphics/llconstantset.h"

#include "graphics/llbuffer.h"
#include "graphics/llimage.h"

#include "vulkan/llconstantset_vk.h"

llama::ConstantSet llama::createConstantSet(Shader shader, uint32_t setIndex, std::initializer_list<ConstantResource> resources)
{
    return std::make_shared<ConstantSet_IVulkan>(std::static_pointer_cast<Shader_IVulkan>(shader), setIndex, resources);
}
