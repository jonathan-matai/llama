#include "llpch.h"
#include "llconstantset_vk.h"

#include "llbuffer_vk.h"

llama::ConstantSet_IVulkan::ConstantSet_IVulkan(std::shared_ptr<Shader_IVulkan> shader, uint32_t setIndex, std::initializer_list<ConstantResource> resources)
{
    m_pool = shader->createPool(setIndex);

    std::vector<vk::DescriptorSetLayout> layouts(shader->getSwapchainSize(), shader->m_setLayouts[setIndex].get());

    assert_vulkan(shader->getDevice().allocateDescriptorSets(vk::DescriptorSetAllocateInfo(m_pool.get(),
                                                                                           static_cast<uint32_t>(layouts.size()), 
                                                                                           layouts.data())),
                  m_sets, LLAMA_DEBUG_INFO, "vk::Device::allocateDescriptorSetsUnique() failed!");

    std::list<vk::DescriptorBufferInfo> bufferInfos;
    std::list<vk::DescriptorImageInfo> imageInfos;
    std::vector<vk::WriteDescriptorSet> writeSets;

    for (uint32_t i = 0; i < shader->getSwapchainSize(); ++i)
    {
        for (uint32_t j = 0; j < resources.size(); ++j)
        {
            llama::ConstantResource resource = *(resources.begin() + j);

            switch (resource->getResourceType())
            {
                case ConstantResource_T::Type::constantBuffer:
                {
                    auto a = std::static_pointer_cast<ConstantBuffer_IVulkan>(resource);

                    bufferInfos.push_back(vk::DescriptorBufferInfo(a->m_buffer.first, // Buffer
                                                                   a->offset(0, i), // Offset
                                                                   a->m_alignedSize /* Size */));

                    writeSets.push_back(vk::WriteDescriptorSet(m_sets[i], // Descriptor Set
                                                               j, // Binding
                                                               0, // Array Element
                                                               1, // Descriptor Count
                                                               a->m_elementCount > 1  ? vk::DescriptorType::eUniformBufferDynamic : vk::DescriptorType::eUniformBuffer,
                                                               nullptr, // Image Info
                                                               &*bufferInfos.rbegin(), // Buffer Info
                                                               nullptr /* Texel Buffer View */));

                    break;
                }

                case ConstantResource_T::Type::constantArrayBuffer:
                {
                    auto a = static_cast<ConstantBuffer_IVulkan*>(std::static_pointer_cast<ConstantArrayBuffer_T>(resource)->getBuffer());

                    bufferInfos.push_back(vk::DescriptorBufferInfo(a->m_buffer.first, // Buffer
                                                                   a->offset(0, i), // Offset
                                                                   a->m_alignedSize /* Size */));

                    writeSets.push_back(vk::WriteDescriptorSet(m_sets[i], // Descriptor Set
                                                               j, // Binding
                                                               0, // Array Element
                                                               1, // Descriptor Count
                                                               vk::DescriptorType::eUniformBufferDynamic,
                                                               nullptr, // Image Info
                                                               &*bufferInfos.rbegin(), // Buffer Info
                                                               nullptr /* Texel Buffer View */));

                    break;
                }

                case ConstantResource_T::Type::sampler:
                {
                    auto a = std::static_pointer_cast<SampledImage_IVulkan>(resource);

                    imageInfos.push_back(vk::DescriptorImageInfo(a->m_sampler.get(),
                                                                 a->m_imageView,
                                                                 vk::ImageLayout::eShaderReadOnlyOptimal));

                    writeSets.push_back(vk::WriteDescriptorSet(m_sets[i], // Descriptor Set
                                                               j, // Binding
                                                               0, // Array Element
                                                               1, // Descriptor Count
                                                               vk::DescriptorType::eCombinedImageSampler,
                                                               &*imageInfos.rbegin(), // Image Info
                                                               nullptr, // Buffer Info
                                                               nullptr /* Texel Buffer View */));

                    break;
                }

            }
        }
    }

    shader->getDevice().updateDescriptorSets(writeSets, { });
    
}