#pragma once

#include "llgraphics_vk.h"
#include "graphics/llconstantset.h"
#include "llshader_vk.h"

namespace llama
{
    class ConstantSet_IVulkan : public ConstantSet_T
    {
        friend class Renderer_IVulkan;

    public:

        ConstantSet_IVulkan(std::shared_ptr<Shader_IVulkan> shader, uint32_t setIndex, std::initializer_list<ConstantResource> resources);

    private:

        // static vk::DescriptorType getVkDescriptorType(ConstantResource_T::Type type);

        vk::UniqueDescriptorPool m_pool;
        std::vector<vk::UniqueDescriptorSet> m_sets;
    };
}