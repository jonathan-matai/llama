#include "llpch.h"
#include "llshader_vk.h"

#include <fstream>

llama::Shader_IVulkan::Shader_IVulkan(std::shared_ptr<WindowContext_IVulkan> context, std::string_view vertexShaderSpv, std::string_view fragmentShaderSpv)
{
    createLayout(context->getDevice());

    vk::UniqueShaderModule vertexModule = createShaderModule(context->getDevice(), vertexShaderSpv);
    vk::UniqueShaderModule fragmentModule = createShaderModule(context->getDevice(), fragmentShaderSpv);

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages
    {
        vk::PipelineShaderStageCreateInfo({}, // flags
                                          vk::ShaderStageFlagBits::eVertex,
                                          vertexModule.get(),
                                          "main"),
        vk::PipelineShaderStageCreateInfo({}, // flags
                                          vk::ShaderStageFlagBits::eFragment,
                                          fragmentModule.get(),
                                          "main")
    };

    vk::PipelineVertexInputStateCreateInfo vertexInput_ci({}, // flags
                                                          0, nullptr, // Vertex Bindings
                                                          0, nullptr /* Vertex Attributes */);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly_ci({}, // Flags
                                                              vk::PrimitiveTopology::eTriangleList, // Topology
                                                              VK_FALSE /* Topology Restart */);

    vk::PipelineViewportStateCreateInfo viewport_ci({}, // Flags
                                                    1, nullptr,
                                                    1, nullptr);

    vk::PipelineRasterizationStateCreateInfo rasterization_ci({}, // Flags
                                                              VK_FALSE, // Depth Clamp
                                                              VK_FALSE, // Rasterizer Discard
                                                              vk::PolygonMode::eFill,
                                                              vk::CullModeFlagBits::eNone,
                                                              vk::FrontFace::eClockwise,
                                                              VK_FALSE, 0.0f, 0.0f, 0.0f, // Depth Bias
                                                              5 /* Line Width */);

    vk::PipelineMultisampleStateCreateInfo multisample_ci({}, // Flags
                                                          vk::SampleCountFlagBits::e4,
                                                          VK_FALSE, // Sample Shading
                                                          0.5f, // Min Sample Shading
                                                          nullptr, // Sample Mask
                                                          VK_FALSE, // Alpha to Coverage
                                                          VK_FALSE/* Alpha to One */);

    vk::PipelineDepthStencilStateCreateInfo depthStencil_ci({}, // Flags
                                                            VK_TRUE, // Depth Test
                                                            VK_TRUE, // Depth Write
                                                            vk::CompareOp::eLess,
                                                            VK_FALSE, // Depth Bounds Test
                                                            VK_FALSE, // Stencil Test
                                                            {}, // Stencil Front
                                                            {}, // Stencil Back
                                                            0.0f, // Min Depth
                                                            1.0f /* Max Depth */);

    vk::PipelineColorBlendAttachmentState blendAttachment(VK_FALSE, // Blending enabled
                                                          vk::BlendFactor::eSrcAlpha, // Source Color Blend Factor
                                                          vk::BlendFactor::eOneMinusSrcAlpha, // Destination Color Blend Factor
                                                          vk::BlendOp::eAdd, // Color Blend Operation
                                                          vk::BlendFactor::eOne, // Source Alpha Blend Factor
                                                          vk::BlendFactor::eZero, // Destination Alpha Blend Factor
                                                          vk::BlendOp::eAdd, // Alpha Blend Operation
                                                          vk::ColorComponentFlagBits::eA | // Blend Factors
                                                          vk::ColorComponentFlagBits::eR | 
                                                          vk::ColorComponentFlagBits::eG | 
                                                          vk::ColorComponentFlagBits::eB);

    vk::PipelineColorBlendStateCreateInfo colorBlend_ci({}, // Flags
                                                        VK_FALSE, // Logic OP
                                                        vk::LogicOp::eNoOp, // Logiv OP
                                                        1, &blendAttachment,
                                                        { 0.0f, 0.0f, 0.0f, 0.0f });

    std::array<vk::DynamicState, 2> dynamicStates
    {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState_ci({}, // Flags
                                                       static_cast<uint32_t>(dynamicStates.size()),
                                                       dynamicStates.data());

    assert_vulkan(context->getDevice().createGraphicsPipelineUnique({},
                                                                    vk::GraphicsPipelineCreateInfo({}, // Flags
                                                                                                   static_cast<uint32_t>(shaderStages.size()),
                                                                                                   shaderStages.data(),
                                                                                                   &vertexInput_ci,
                                                                                                   &inputAssembly_ci,
                                                                                                   nullptr,
                                                                                                   &viewport_ci,
                                                                                                   &rasterization_ci,
                                                                                                   &multisample_ci,
                                                                                                   &depthStencil_ci,
                                                                                                   &colorBlend_ci,
                                                                                                   &dynamicState_ci,
                                                                                                   m_layout.get(),
                                                                                                   context->getRenderPass(),
                                                                                                   0, // Subpass
                                                                                                   nullptr, 0 /* Base Pipeline */)),
                  m_pipeline, LLAMA_DEBUG_INFO, "vk::Device::createGraphicsPipelineUnique() failed!");
}

bool llama::Shader_IVulkan::createLayout(vk::Device device)
{
    if (!assert_vulkan(device.createPipelineLayoutUnique(vk::PipelineLayoutCreateInfo({}, // Flags
                                                                                      0, nullptr, // Descriptior Set Layouts
                                                                                      0, nullptr /* Push Constants */)),
                       m_layout, LLAMA_DEBUG_INFO, "vk::Device::createPipelineLayoutUnique() failed!"))
        return false;

    return true;
}

vk::UniqueShaderModule llama::Shader_IVulkan::createShaderModule(vk::Device device, std::string_view spvPath)
{
    // Open File at End
    std::ifstream spvFile(std::string(spvPath).c_str(), std::ios::ate | std::ios::binary);

    if (spvFile.fail())
        logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Couldn't open File \"%*s\"", spvPath.size(), spvPath.data());

    // Get current positon -> Size of the file
    std::streamsize spvSize = spvFile.tellg();

    // Got to start of file
    spvFile.seekg(0, std::ios::beg);

    // Read file
    std::vector<char> spvData(spvSize);
    spvFile.read(spvData.data(), spvData.size());

    // Close file
    spvFile.close();

    vk::UniqueShaderModule module;

    assert_vulkan(device.createShaderModuleUnique(vk::ShaderModuleCreateInfo({}, /* flags*/ spvData.size(), reinterpret_cast<uint32_t*>(spvData.data()))),
                  module, LLAMA_DEBUG_INFO, "vk::Device::createShaderModuleUnique() failed!");

    return module;
}
