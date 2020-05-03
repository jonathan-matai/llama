#pragma once

#include "llrenderer_vk.h"

#include "graphics/llshader.h"

#include <json.h>

namespace llama
{
    class Renderer_IVulkan;

    class Shader_IVulkan : public Shader_T
    {
        friend class ConstantSet_IVulkan;

    public:

        Shader_IVulkan(std::shared_ptr<Renderer_IVulkan> renderer, std::string_view descriptorFilePath);
        ~Shader_IVulkan() { }

        inline vk::Pipeline getPipeline() const { return m_pipeline.get(); }
        inline vk::PipelineLayout getPipelineLayout() const { return m_layout.get(); }
        inline vk::Device getDevice() const { return m_renderer->getDevice(); }
        inline uint32_t getSwapchainSize() const { return m_renderer->getSwapchainSize(); }

        vk::UniqueDescriptorPool createPool(uint32_t set = 0, uint32_t setCount = 1);

    private:

        std::pair<std::vector<vk::PipelineShaderStageCreateInfo>, std::vector<vk::UniqueShaderModule>> createShaderStages(Json::Value& file);
        std::pair<vk::PipelineVertexInputStateCreateInfo, std::pair<std::vector<vk::VertexInputBindingDescription>, std::vector<vk::VertexInputAttributeDescription>>> 
            createVertexInputState(Json::Value& file);
        vk::PipelineInputAssemblyStateCreateInfo createInputAssemlyState(Json::Value& file);
        vk::PipelineRasterizationStateCreateInfo createRasterizationState(Json::Value& file);
        bool createLayout(Json::Value& file, vk::Device device);

        static vk::Format variableTypeStringToVkFormat(std::string_view type);
        static vk::DescriptorType descriptorTypeStringtoVkDesriptorType(std::string_view type);
        static vk::ShaderStageFlagBits shaderStageStringToVkShaderStage(std::string_view stage);

        static vk::UniqueShaderModule createShaderModule(vk::Device device, std::string_view spvPath);

        std::string m_filename;
        std::shared_ptr<Renderer_IVulkan> m_renderer;
        std::vector<vk::UniqueDescriptorSetLayout> m_setLayouts;
        std::vector<std::vector<vk::DescriptorPoolSize>> m_poolSizes;
        vk::UniquePipelineLayout m_layout;
        vk::UniquePipeline m_pipeline;
    };
}