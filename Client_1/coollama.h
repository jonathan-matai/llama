#pragma once

#include "llama.h"

class CoolLlama : public llama::RenderableEntity
{
public:

    struct ConstantBufferData
    {
        llama::float2 m_position;
        llama::float3x3 m_rotation;
    };

    static void createStaticResources(llama::GraphicsDevice device, llama::Renderer renderer);
    
    CoolLlama(uint64_t uniqueID, llama::float2 pos) :
        RenderableEntity(uniqueID, s_shader, s_vertexBuffer, s_constantSet, s_arrayBuffer)
    {
    }

    void onTick(float delta) override
    {
        s_arrayBuffer->at<ConstantBufferData>(m_arrayIndex).m_position = m_position;
        s_arrayBuffer->at<ConstantBufferData>(m_arrayIndex).m_rotation = llama::float3x3();
    }


private:

    llama::float2 m_position;

    static llama::VertexBuffer s_vertexBuffer;
    static llama::SampledImage s_sampledImage;
    static llama::Shader s_shader;
    static llama::ConstantArrayBuffer s_arrayBuffer;
    static llama::ConstantSet s_constantSet;
};

// IMPLEMENTATION

void CoolLlama::createStaticResources(llama::GraphicsDevice device, llama::Renderer renderer)
{
    std::vector<llama::float4> verticies
    {
        llama::float4(-.15f, -.3f, 0.0f, 0.0f),
        llama::float4(0.15f, -.3f, 1.0f, 0.0f),
        llama::float4(0.15f, 0.3f, 1.0f, 1.0f),
        llama::float4(0.15f, 0.3f, 1.0f, 1.0f),
        llama::float4(-.15f, 0.3f, 0.0f, 1.0f),
        llama::float4(-.15f, -.3f, 0.0f, 0.0f),
    };

    s_vertexBuffer = llama::createVertexBuffer(device, sizeof(llama::float4) * verticies.size(), verticies.data());
    s_sampledImage = llama::createSampledImage(device, "resources/textures/llama.png");
    s_shader = llama::createShader(renderer, "resources/shaders/image2d/image2d.json");
    s_arrayBuffer = llama::createConstantArrayBuffer(device, sizeof(ConstantBufferData), 100, 3);
    s_constantSet = llama::createConstantSet(s_shader, 0, { s_arrayBuffer, s_sampledImage });
}