#include "llpch.h"
#include "llshader_vk.h"

#include <filesystem>
#include <fstream>

#include "math/llmath.h"

llama::Shader_IVulkan::Shader_IVulkan(std::shared_ptr<Renderer_IVulkan> renderer, std::string_view descriptorFilePath) :
    m_renderer(renderer),
    m_filename(descriptorFilePath)
{
    std::ifstream descriptorFile(std::string(descriptorFilePath).c_str());

    if (!descriptorFile.is_open())
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Couldn't open file \"%*s\" for creating Shader!", descriptorFilePath.length(), descriptorFilePath.data());
        throw std::runtime_error("Couldn't open file, see Logfile for more info");
    }

    Json::Value jsonFile;
    descriptorFile >> jsonFile;

    

    createLayout(jsonFile, renderer->getDevice());

    auto shaderStages = createShaderStages(jsonFile);

    auto vertexInput_ci = createVertexInputState(jsonFile);

    auto inputAssembly_ci = createInputAssemlyState(jsonFile);

    vk::PipelineViewportStateCreateInfo viewport_ci({}, // Flags
                                                    1, nullptr,
                                                    1, nullptr);

    auto rasterization_ci = createRasterizationState(jsonFile);

    float sampleShading = clamp(0.0f, jsonFile["smoothTextureLevel"].isNumeric() ? jsonFile["smoothTextureLevel"].asFloat() : 0.0f, 1.0f);

    vk::PipelineMultisampleStateCreateInfo multisample_ci({}, // Flags
                                                          vk::SampleCountFlagBits::e4,
                                                          sampleShading > 0.0f, // Sample Shading
                                                          sampleShading, // Min Sample Shading
                                                          nullptr, // Sample Mask
                                                          VK_FALSE, // Alpha to Coverage
                                                          VK_FALSE/* Alpha to One */);

    bool depthTest = jsonFile["depthTest"].isBool() ? jsonFile["depthTest"].asBool() : true;

    vk::PipelineDepthStencilStateCreateInfo depthStencil_ci({}, // Flags
                                                            depthTest, // Depth Test
                                                            depthTest, // Depth Write
                                                            vk::CompareOp::eLess,
                                                            VK_FALSE, // Depth Bounds Test
                                                            VK_FALSE, // Stencil Test
                                                            {}, // Stencil Front
                                                            {}, // Stencil Back
                                                            0.0f, // Min Depth
                                                            1.0f /* Max Depth */);

    bool blending = jsonFile["blending"].isBool() ? jsonFile["blending"].asBool() : false;

    vk::PipelineColorBlendAttachmentState blendAttachment(blending, // Blending enabled
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

    assert_vulkan(renderer->getDevice().createGraphicsPipelineUnique({},
                                                                     vk::GraphicsPipelineCreateInfo({}, // Flags
                                                                                                    static_cast<uint32_t>(shaderStages.first.size()),
                                                                                                    shaderStages.first.data(),
                                                                                                    &vertexInput_ci.first,
                                                                                                    &inputAssembly_ci,
                                                                                                    nullptr,
                                                                                                    &viewport_ci,
                                                                                                    &rasterization_ci,
                                                                                                    &multisample_ci,
                                                                                                    &depthStencil_ci,
                                                                                                    &colorBlend_ci,
                                                                                                    &dynamicState_ci,
                                                                                                    m_layout.get(),
                                                                                                    renderer->getRenderPass(),
                                                                                                    0, // Subpass
                                                                                                    nullptr, 0 /* Base Pipeline */)),
                  m_pipeline, LLAMA_DEBUG_INFO, "vk::Device::createGraphicsPipelineUnique() failed!");
}

vk::UniqueDescriptorPool llama::Shader_IVulkan::createPool(uint32_t set, uint32_t setCount)
{
    vk::UniqueDescriptorPool pool;

    assert_vulkan(m_renderer->getDevice().createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo({}, // Flags
                                                                                                  setCount * m_renderer->getSwapchainSize(), // Max Sets
                                                                                                  static_cast<uint32_t>(m_poolSizes[set].size()),
                                                                                                  m_poolSizes[set].data())),
                  pool, LLAMA_DEBUG_INFO, "vk::Device::createDescriptorPoolUnique() failed!");

    return pool;
}

std::pair<std::vector<vk::PipelineShaderStageCreateInfo>, std::vector<vk::UniqueShaderModule>> llama::Shader_IVulkan::createShaderStages(Json::Value& file)
{
    size_t pathEnd = m_filename.find_last_of("\\/");
    std::string path = pathEnd == std::string::npos ? m_filename : m_filename.substr(0, pathEnd + 1);

    std::pair<std::vector<vk::PipelineShaderStageCreateInfo>, std::vector<vk::UniqueShaderModule>> stages;

    if (file["modules"]["vertex"].isString())
    {
        stages.second.push_back(createShaderModule(m_renderer->getDevice(), path + file["modules"]["vertex"].asString()));
        stages.first.push_back(vk::PipelineShaderStageCreateInfo({}, // flags
                                                                 vk::ShaderStageFlagBits::eVertex,
                                                                 (*stages.second.rbegin()).get(),
                                                                 "main"));
    }
    else
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" doesn't contain the required Value \"modules.vertex\"!", m_filename.c_str());
        throw std::runtime_error("Invalid file, see Logfile for more info");
    }

    if (file["modules"]["fragment"].isString())
    {
        stages.second.push_back(createShaderModule(m_renderer->getDevice(), path + file["modules"]["fragment"].asString()));
        stages.first.push_back(vk::PipelineShaderStageCreateInfo({}, // flags
                                                                 vk::ShaderStageFlagBits::eFragment,
                                                                 (*stages.second.rbegin()).get(),
                                                                 "main"));
    }
    else
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" doesn't contain the required Value \"modules.fragment\"!", m_filename.c_str());
        throw std::runtime_error("Invalid file, see Logfile for more info");
    }

    return stages;
}

std::pair<
    vk::PipelineVertexInputStateCreateInfo,
    std::pair<
    std::vector<vk::VertexInputBindingDescription>,
    std::vector<vk::VertexInputAttributeDescription>
    >> llama::Shader_IVulkan::createVertexInputState(Json::Value& file)
{
    std::vector<vk::VertexInputBindingDescription> vertexBindings;
    std::vector<vk::VertexInputAttributeDescription> vertexAttributes;

    if (file["vertexBindings"].isArray())
    {
        uint32_t size = file["vertexBindings"].size();

        for (uint32_t i = 0; i < size; ++i)
        {
            if (!file["vertexBindings"][i]["locations"].isArray() || file["vertexBindings"][i]["locations"].size() == 0)
            {
                LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has no Locations at Vertex Binding %d", m_filename.c_str(), i));
                continue;
            }

            uint32_t stride = 0;

            if (file["vertexBindings"][i]["size"].isIntegral())
            {
                stride = file["vertexBindings"][i]["size"].asUInt();

                if (stride == 0)
                {
                    LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has a Size of 0 at Vertex Binding %d", m_filename.c_str(), i));
                    continue;
                }
            }                
            else
            {
                LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has no Size at Vertex Binding %d", m_filename.c_str(), i));
                continue;
            }

            vk::VertexInputRate rate = vk::VertexInputRate::eVertex;

            if (file["vertexBindings"][i]["type"].isString())
            {
                std::string type = file["vertexBindings"][i]["type"].asString();

                if (type == "vertex")
                    rate = vk::VertexInputRate::eVertex;
                else if (type == "instance")
                    rate = vk::VertexInputRate::eInstance;
                else
                    LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has an invalid type at Vertex Binding %d", m_filename.c_str(), i));
            }

            uint32_t isize = file["vertexBindings"][i]["locations"].size();

            vertexBindings.push_back(vk::VertexInputBindingDescription(i, stride, rate));

            for (uint32_t j = 0; j < isize; ++j)
            {
                vk::Format format = vk::Format::eUndefined;

                if (file["vertexBindings"][i]["locations"][j]["type"].isString())
                {
                    std::string type = file["vertexBindings"][i]["locations"][j]["type"].asString();
                    format = variableTypeStringToVkFormat(type);

                    if (format == vk::Format::eUndefined)
                    {
                        LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has an Invalid Type at Vertex Binding %d and Vertex Location %d", 
                                                          m_filename.c_str(), i, j));
                        continue;
                    }
                }
                else
                {
                    LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has no Type at Vertex Binding %d and Vertex Location %d",
                                                      m_filename.c_str(), i, j));
                    continue;
                }

                uint32_t offset = 0;
                
                if (file["vertexBindings"][i]["locations"][j]["offset"].isIntegral())
                {
                    offset = file["vertexBindings"][i]["locations"][j]["offset"].asUInt();
                }
                else
                {
                    LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Shader Descriptor file \"%s\" has no Size at Vertex Binding %d and Vertex Location %d",
                                                      m_filename.c_str(), i, j));
                }

                vertexAttributes.push_back(vk::VertexInputAttributeDescription(j, // Location
                                                                               i, // Binding
                                                                               format, // Format
                                                                               offset));
            }
        }
    
    }

    auto vectors = std::make_pair(std::move(vertexBindings), std::move(vertexAttributes));

    return std::make_pair(vk::PipelineVertexInputStateCreateInfo({}, // flags
                                                                 static_cast<uint32_t>(vectors.first.size()), vectors.first.data(), // Vertex Bindings
                                                                 static_cast<uint32_t>(vectors.second.size()), vectors.second.data() /* Vertex Attributes */),
                          std::move(vectors));
}

vk::PipelineInputAssemblyStateCreateInfo llama::Shader_IVulkan::createInputAssemlyState(Json::Value& file)
{
    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;

    if (file["vertexTopology"].isString())
    {
        std::string top = file["vertexTopology"].asString();

        if (top == "pointList")                        topology = vk::PrimitiveTopology::ePointList;
        else if (top == "lineList")                    topology = vk::PrimitiveTopology::eLineList;
        else if (top == "lineStrip")                   topology = vk::PrimitiveTopology::eLineStrip;
        else if (top == "triangleList")                topology = vk::PrimitiveTopology::eTriangleList;
        else if (top == "triangleStrip")               topology = vk::PrimitiveTopology::eTriangleStrip;
        else if (top == "triangleFan")                 topology = vk::PrimitiveTopology::eTriangleFan;
        else if (top == "lineListWithAdjacency")       topology = vk::PrimitiveTopology::eLineListWithAdjacency;
        else if (top == "lineStripWithAdjacency")      topology = vk::PrimitiveTopology::eLineStripWithAdjacency;
        else if (top == "triangleListWithAdjacency")   topology = vk::PrimitiveTopology::eTriangleListWithAdjacency;
        else if (top == "triangleStripWithAdjacency")  topology = vk::PrimitiveTopology::eTriangleStripWithAdjacency;
        else if (top == "patchList")                   topology = vk::PrimitiveTopology::ePatchList;
    }

    return vk::PipelineInputAssemblyStateCreateInfo({},
                                                    topology, // Topology
                                                    VK_FALSE /* Primitive Restart */);
}

vk::PipelineRasterizationStateCreateInfo llama::Shader_IVulkan::createRasterizationState(Json::Value& file)
{
    vk::PolygonMode polygonMode = vk::PolygonMode::eFill;

    if (file["polygonMode"].isString())
    {
        std::string polyMode = file["polygonMode"].asString();

        if (polyMode == "fill")  polygonMode = vk::PolygonMode::eFill;
        if (polyMode == "line")  polygonMode = vk::PolygonMode::eLine;
        if (polyMode == "point") polygonMode = vk::PolygonMode::ePoint;
    }

    bool culling = file["backfaceCulling"].isBool() ? file["backfaceCulling"].asBool() : false;

    return vk::PipelineRasterizationStateCreateInfo({}, // Flags
                                                    VK_FALSE, // Depth Clamp
                                                    VK_FALSE, // Rasterizer Discard
                                                    polygonMode,
                                                    culling ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone,
                                                    vk::FrontFace::eClockwise,
                                                    VK_FALSE, 0.0f, 0.0f, 0.0f, // Depth Bias
                                                    file["lineWidth"].isNumeric() ? file["lineWidth"].asFloat() : 1.0f /* Line Width */);
}

bool llama::Shader_IVulkan::createLayout(Json::Value& file, vk::Device device)
{
    if (file["descriptorBindings"].isArray())
    {
        uint32_t size = static_cast<uint32_t>(file["descriptorBindings"].size());
        m_poolSizes.resize(size);


        for (uint32_t i = 0; i < size; ++i)
        {
            if (!file["descriptorBindings"][i].isArray())
            {
                LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, "Descriptor Binding %d is not an Array!", i));
                continue;
            }

            std::vector<vk::DescriptorSetLayoutBinding> bindings;

            uint32_t isize = static_cast<uint32_t>(file["descriptorBindings"][i].size());

            for (uint32_t j = 0; j < isize; ++j)
            {
                if (!file["descriptorBindings"][i][j]["type"].isString())
                {
                    LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, "Descriptor Binding %d/%d does not have a Type!", i, j));
                    continue;
                }

                vk::DescriptorType type = descriptorTypeStringtoVkDesriptorType(file["descriptorBindings"][i][j]["type"].asString());

                if (!file["descriptorBindings"][i][j]["stage"].isString())
                {
                    LLAMA_DEBUG_ONLY(logfile()->print(Colors::YELLOW, "Descriptor Binding %d/%d is not have a Stage!", i, j));
                    continue;
                }

                auto stage = shaderStageStringToVkShaderStage(file["descriptorBindings"][i][j]["stage"].asString());

                auto result = std::find_if(m_poolSizes[i].begin(), m_poolSizes[i].end(), [type](const vk::DescriptorPoolSize value) { return type == value.type; });

                if (result == m_poolSizes[i].end())
                    m_poolSizes[i].push_back(vk::DescriptorPoolSize(type, getSwapchainSize()));
                else 
                    result->descriptorCount += getSwapchainSize();

                bindings.push_back(vk::DescriptorSetLayoutBinding(j, type, 1, stage, nullptr));
            }

            vk::UniqueDescriptorSetLayout layout;

            if (!assert_vulkan(m_renderer->getDevice().createDescriptorSetLayoutUnique(vk::DescriptorSetLayoutCreateInfo({},
                                                                                                                         static_cast<uint32_t>(bindings.size()),
                                                                                                                         bindings.data())),
                             layout, LLAMA_DEBUG_INFO, "vk::Device::createDescriptorSetLayoutUnique() failed!"))
               continue;

           m_setLayouts.push_back(std::move(layout));
        }
    }

    // Copy to vector of base Objects (not unique pointers)

    std::vector<vk::DescriptorSetLayout> layouts(m_setLayouts.size());

    for (size_t i = 0; i < m_setLayouts.size(); ++i)
        layouts[i] = m_setLayouts[i].get();

    if (!assert_vulkan(device.createPipelineLayoutUnique(vk::PipelineLayoutCreateInfo({}, // Flags
                                                                                      static_cast<uint32_t>(layouts.size()), layouts.data(), // Descriptior Set Layouts
                                                                                      0, nullptr /* Push Constants */)),
                       m_layout, LLAMA_DEBUG_INFO, "vk::Device::createPipelineLayoutUnique() failed!"))
        return false;

    return true;
}

vk::Format llama::Shader_IVulkan::variableTypeStringToVkFormat(std::string_view type)
{
    if (type.substr(0, 3) == "int")
    {
        char modifier = type.at(3);

        switch (modifier)
        {
        case '\0':
        case '1':
            return vk::Format::eR32Sint;
        case '2':
            return vk::Format::eR32G32Sint;
        case '3':
            return vk::Format::eR32G32B32Sint;
        case '4':
            return vk::Format::eR32G32B32A32Sint;
        default:
            return vk::Format::eUndefined;
        }
    }
    else if (type.substr(0, 4) == "uint")
    {
        char modifier = type.at(4);

        switch (modifier)
        {
        case '\0':
        case '1':
            return vk::Format::eR32Uint;
        case '2':
            return vk::Format::eR32G32Uint;
        case '3':
            return vk::Format::eR32G32B32Uint;
        case '4':
            return vk::Format::eR32G32B32A32Uint;
        default:
            return vk::Format::eUndefined;
        }
    }
    else if (type.substr(0, 5) == "float")
    {
        char modifier = type.at(5);

        switch (modifier)
        {
        case '\0':
        case '1':
            return vk::Format::eR32Sfloat;
        case '2':
            return vk::Format::eR32G32Sfloat;
        case '3':
            return vk::Format::eR32G32B32Sfloat;
        case '4':
            return vk::Format::eR32G32B32A32Sfloat;
        default:
            return vk::Format::eUndefined;
        }
    }

    return vk::Format::eUndefined;
}

vk::DescriptorType llama::Shader_IVulkan::descriptorTypeStringtoVkDesriptorType(std::string_view type)
{
    if (type == "constantBuffer")
        return vk::DescriptorType::eUniformBuffer;
    if (type == "constantArrayBuffer")
        return vk::DescriptorType::eUniformBufferDynamic;
    if (type == "storageBuffer")
        return vk::DescriptorType::eStorageBuffer;
    if (type == "storageArrayBuffer")
        return vk::DescriptorType::eStorageBufferDynamic;
    if (type == "sampledImage")
        return vk::DescriptorType::eCombinedImageSampler;

    logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "Invalid Descriptor Type \"%*s\"", type.size(), type.data());

    return vk::DescriptorType();
}

vk::ShaderStageFlagBits llama::Shader_IVulkan::shaderStageStringToVkShaderStage(std::string_view stage)
{
    if (stage == "vertex")
        return vk::ShaderStageFlagBits::eVertex;
    if (stage == "fragment")
        return vk::ShaderStageFlagBits::eFragment;

    logfile()->print(Colors::RED, "Invalid Shader Stage \"%*s\"", stage.size(), stage.data());

    return vk::ShaderStageFlagBits::eVertex;
}

vk::UniqueShaderModule llama::Shader_IVulkan::createShaderModule(vk::Device device, std::string_view spvPath)
{
    // Open File at End
    std::ifstream spvFile(std::string(spvPath).c_str(), std::ios::ate | std::ios::binary);

    if (!spvFile.is_open())
    {
        logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Couldn't open File \"%*s\"", spvPath.size(), spvPath.data());
        throw std::runtime_error("Couldn't open file, see Logfile for more info");
    }

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
