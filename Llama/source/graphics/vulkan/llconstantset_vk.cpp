#include "llpch.h"
#include "llconstantset_vk.h"

llama::ConstantSet_IVulkan::ConstantSet_IVulkan(std::shared_ptr<Shader_IVulkan> shader, uint32_t setIndex, std::initializer_list<ConstantResource> resources)
{
    m_pool = shader->createPool(setIndex);

    std::vector<vk::DescriptorSetLayout> layouts(shader->getSwapchainSize(), shader->m_setLayouts[setIndex].get());

    assert_vulkan(shader->getDevice().allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo(m_pool.get(),
                                                                                                 static_cast<uint32_t>(layouts.size()), 
                                                                                                 layouts.data())),
                  m_sets, LLAMA_DEBUG_INFO, "vk::Device::allocateDescriptorSetsUnique() failed!");

    std::list<vk::DescriptorBufferInfo> bufferInfos;
    std::vector<vk::WriteDescriptorSet> writeSets;

    for (uint32_t i = 0; i < shader->getSwapchainSize(); ++i)
    {
        for (uint32_t j = 0; j < resources.size(); ++j)
        {
            llama::ConstantResource resource = *(resources.begin() + j);

            switch (resource->getResourceType())
            {
            case ConstantResource_T::Type::constantBuffer:
            case ConstantResource_T::Type::constantArrayBuffer:

                auto a = std::static_pointer_cast<ConstantBuffer_IVulkan>(resource);

                bufferInfos.push_back(vk::DescriptorBufferInfo(a->m_buffer.first, // Buffer
                                                               a->offset(0, i), // Offset
                                                               a->size() /* Size */ ));

                writeSets.push_back(vk::WriteDescriptorSet(m_sets[i].get(), // Descriptor Set
                                                           j, // Binding
                                                           0, // Array Element
                                                           1, // Descriptor Count
                                                           a->getResourceType() == ConstantResource_T::Type::constantArrayBuffer ? // Type
                                                               vk::DescriptorType::eUniformBufferDynamic : 
                                                               vk::DescriptorType::eUniformBuffer, 
                                                           nullptr, // Image Info
                                                           &*bufferInfos.rbegin(), // Buffer Info
                                                           nullptr /* Texel Buffer View */ ));
            }
        }
    }

    shader->getDevice().updateDescriptorSets(writeSets, { });
    
}

vk::DescriptorType llama::ConstantSet_IVulkan::getVkDescriptorType(ConstantResource_T::Type type)
{
    using IType = ConstantResource_T::Type;
    using OType = vk::DescriptorType;

    switch (type)
    {
    case IType::sampler:                return OType::eCombinedImageSampler;
    case IType::constantBuffer:         return OType::eUniformBuffer;
    case IType::constantArrayBuffer:    return OType::eUniformBufferDynamic;
    case IType::storageBuffer:          return OType::eStorageBuffer;
    case IType::storageArrayBuffer:     return OType::eStorageBufferDynamic;
    default:                            return OType::eSampler;
    }
}
