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
        RenderableEntity(uniqueID, s_shader, s_vertexBuffer, s_constantSet, s_arrayBuffer),
        m_position(pos),
        up(false),
        clockwise(false),
        do360(false),
        offset(0.0f),
        rotation(0.0f),
        timer360(false)
    {
        float modifier = ((std::rand() % 40) + 80) / 100.0f;
        offsetPms = 0.0001f * modifier;

        modifier = ((std::rand() % 40) + 80) / 100.0f;
        rotPms = 0.002f * modifier;

        modifier = ((std::rand() % 100) + 50) / 100.0f;
        threshold360 = 13000.0f * modifier;
    }

    void onTick(float delta) override
    {
        if (up)
        {
            offset -= offsetPms * delta;

            if (offset < -0.15f)
                up = false;
        }
        else
        {
            offset += offsetPms * delta;

            if (offset > 0.15f)
                up = true;
        }

        if (do360)
        {
            rotation += 0.6f * delta;

            if (rotation > 360.0f)
            {
                rotation -= 360.0f;
                do360 = false;
                timer360 = 0;
            }
        }
        else
        {
            timer360 += delta;

            if (timer360 > threshold360)
            {
                do360 = true;
            }

            if (clockwise)
            {
                rotation += rotPms * delta;

                if (rotation > 10.0f)
                    clockwise = false;
            }
            else
            {
                rotation -= rotPms * delta;

                if (rotation < -10.0f)
                    clockwise = true;
            }
        }

        s_arrayBuffer->at<ConstantBufferData>(m_arrayIndex).m_position = m_position + llama::float2(0, offset);
        s_arrayBuffer->at<ConstantBufferData>(m_arrayIndex).m_rotation = llama::rotation2D(llama::radians(rotation));
        //s_arrayBuffer->at<ConstantBufferData>(m_arrayIndex).m_rotation = llama::float3x3();
    }


private:
    llama::float2 m_position;

    float rotation;
    float offset;

    bool up;
    bool clockwise;
    bool do360;

    float rotPms;
    float offsetPms;
    float timer360;
    float threshold360;

    static llama::VertexBuffer s_vertexBuffer;
    static llama::SampledImage s_sampledImage;
    static llama::ConstantSet s_constantSet;
    static llama::Shader s_shader;
    static llama::ConstantArrayBuffer s_arrayBuffer;
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

    s_vertexBuffer = llama::createVertexBuffer(device, verticies);
    s_sampledImage = llama::createSampledImage(device, "resources/textures/llama.png");
    s_shader = llama::createShader(renderer, "resources/shaders/image2d/image2d.json");
    s_arrayBuffer = llama::createConstantArrayBuffer(renderer, sizeof(ConstantBufferData), 100);
    s_constantSet = llama::createConstantSet(s_shader, 0, { s_arrayBuffer, s_sampledImage });
}

llama::VertexBuffer CoolLlama::s_vertexBuffer;
llama::SampledImage CoolLlama::s_sampledImage;
llama::Shader CoolLlama::s_shader;
llama::ConstantArrayBuffer CoolLlama::s_arrayBuffer;
llama::ConstantSet CoolLlama::s_constantSet;