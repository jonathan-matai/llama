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

    using VertexBuffer = std::shared_ptr<VertexBuffer_T>;

    LLAMA_API VertexBuffer createVertexBuffer(GraphicsDevice device, size_t size, const void* data);

    class IndexBuffer_T
    {
    public:

        virtual ~IndexBuffer_T() = default;

    protected:

        IndexBuffer_T() { }
    };

    using IndexBuffer = std::shared_ptr<IndexBuffer_T>;

    LLAMA_API IndexBuffer createIndexBuffer(GraphicsDevice device, const std::vector<uint16_t>& indices);
    LLAMA_API IndexBuffer createIndexBuffer(GraphicsDevice device, const std::vector<uint32_t>& indices);


    class ConstantBuffer_T : public ConstantResource_T
    {
    public:

        virtual ~ConstantBuffer_T() = default;

        template<typename ElementType>
        ElementType& at(uint32_t element = 0, uint32_t swapchainIndex = 0)
        {
            return *static_cast<ElementType*>(at(element, swapchainIndex));
        }


        virtual void* at(uint32_t element = 0, uint32_t swapchainIndex = 0) = 0;

    protected:

        ConstantBuffer_T() :
            ConstantResource_T(Type::constantBuffer) { }
    };

    using ConstantBuffer = std::shared_ptr<ConstantBuffer_T>;

    LLAMA_API ConstantBuffer createConstantBuffer(GraphicsDevice device, size_t elementSize, uint32_t elementCount = 1, uint32_t swapchainSize = 1);

    class ConstantArrayBuffer_T : public ConstantResource_T
    {
    public:

        virtual ~ConstantArrayBuffer_T() = default;

        template<typename ElementType>
        ElementType& at(uint32_t element = 0, uint32_t swapchainIndex = 0)
        {
            return *static_cast<ElementType*>(at(element, swapchainIndex));
        }

        virtual void* at(uint32_t element = 0, uint32_t swapchainIndex = 0) = 0;

        virtual uint32_t addElement() = 0;
        virtual void removeElement(uint32_t index) = 0;

        virtual ConstantBuffer_T* getBuffer() const = 0;

    protected:

        ConstantArrayBuffer_T() :
            ConstantResource_T(Type::constantArrayBuffer) { }
    };

    using ConstantArrayBuffer = std::shared_ptr<ConstantArrayBuffer_T>;

    LLAMA_API ConstantArrayBuffer createConstantArrayBuffer(GraphicsDevice device, size_t elementSize, uint32_t maxElementCount = 1000, uint32_t swapchainSize = 1);
}