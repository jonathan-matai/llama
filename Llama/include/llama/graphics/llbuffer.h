/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llbuffer.h
 Contains Vertex Buffer, Constant Buffer, etc.
*/

#pragma once

#include "llgraphics.h"
#include "llconstantset.h"

namespace llama
{
    class VertexBuffer_T
    {
    public:

        virtual ~VertexBuffer_T() = default;

    protected:

        VertexBuffer_T() { }
    };

    typedef std::shared_ptr<VertexBuffer_T> VertexBuffer;

    LLAMA_API VertexBuffer createVertexBuffer(GraphicsDevice device, size_t size, const void* data);

    class IndexBuffer_T
    {
    public:

        virtual ~IndexBuffer_T() = default;

    protected:

        IndexBuffer_T() { }
    };

    typedef std::shared_ptr<IndexBuffer_T> IndexBuffer;

    LLAMA_API IndexBuffer createIndexBuffer(GraphicsDevice device, const std::vector<uint16_t>& indices);
    LLAMA_API IndexBuffer createIndexBuffer(GraphicsDevice device, const std::vector<uint32_t>& indices);


    class ConstantBuffer_T : public ConstantResource_T
    {
    public:

        virtual ~ConstantBuffer_T() = default;

        virtual void* at(uint32_t element = 0, uint32_t swapchainIndex = 0) = 0;
        virtual size_t offset(uint32_t element = 0, uint32_t swapchainIndex = 0) = 0;

    protected:

        ConstantBuffer_T(Type resourceType) :
            ConstantResource_T(resourceType) { }
    };

    typedef std::shared_ptr<ConstantBuffer_T> ConstantBuffer;

    LLAMA_API ConstantBuffer createConstantBuffer(GraphicsDevice device, size_t elementSize, uint32_t elementCount = 1, uint32_t swapchainSize = 1);
}