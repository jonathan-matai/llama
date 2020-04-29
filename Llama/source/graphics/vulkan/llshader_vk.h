#pragma once

#include "graphics/llshader.h"

namespace llama
{
    class Shader_IVulkan;
}

#include "llrenderer_vk.h"

namespace llama
{
    class Shader_IVulkan : public Shader_T
    {
    public:

        Shader_IVulkan(std::shared_ptr<Renderer_IVulkan> context, std::string_view vertexShaderSpv, std::string_view fragmentShaderSpv);
        ~Shader_IVulkan() { }

        vk::Pipeline getPipeline() const { return m_pipeline.get(); }
        vk::PipelineLayout getPipelineLayout() const { return m_layout.get(); }

    private:

        bool createLayout(vk::Device device);

        static vk::UniqueShaderModule createShaderModule(vk::Device device, std::string_view spvPath);

        vk::UniquePipelineLayout m_layout;
        vk::UniquePipeline m_pipeline;
    };
}