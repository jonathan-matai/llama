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

#include "llconstantresource.h"

namespace llama
{
    LLAMA_CLASS_DECLARATION(Renderer);
    LLAMA_CLASS_DECLARATION(GraphicsDevice);

    class VertexBuffer_T
    {
    public:

        virtual ~VertexBuffer_T() = default;

    protected:

        VertexBuffer_T() { }
    };

    LLAMA_CLASS_DECLARATION(VertexBuffer);

    LLAMA_API VertexBuffer createVertexBuffer(GraphicsDevice device, size_t size, const void* data);

    class IndexBuffer_T
    {
    public:

        virtual ~IndexBuffer_T() = default;

    protected:

        IndexBuffer_T() { }
    };

    LLAMA_CLASS_DECLARATION(IndexBuffer);

    LLAMA_API IndexBuffer createIndexBuffer(GraphicsDevice device, const std::vector<uint16_t>& indices);
    LLAMA_API IndexBuffer createIndexBuffer(GraphicsDevice device, const std::vector<uint32_t>& indices);


    class ConstantBuffer_T : public ConstantResource_T
    {
    public:

        virtual ~ConstantBuffer_T() = default;

        template<typename ElementType>
        ElementType& at(uint32_t element = 0)
        {
            return *static_cast<ElementType*>(at(element));
        }

        virtual void* at(uint32_t element, bool forceIndex = false) = 0;

    protected:

        ConstantBuffer_T() :
            ConstantResource_T(Type::constantBuffer) { }
    };

    LLAMA_CLASS_DECLARATION(ConstantBuffer);

    LLAMA_API ConstantBuffer createConstantBuffer(Renderer renderer, size_t elementSize, uint32_t elementCount = 1);



    class ConstantArrayBuffer_T : public ConstantResource_T
    {
    public:

        virtual ~ConstantArrayBuffer_T() = default;

        template<typename ElementType>
        ElementType& at(uint32_t element = 0)
        {
            return *static_cast<ElementType*>(at(element));
        }

        virtual void* at(uint32_t element = 0) = 0;

        virtual uint32_t addElement() = 0;
        virtual void removeElement(uint32_t index) = 0;

        virtual ConstantBuffer_T* getBuffer() const = 0;

    protected:

        ConstantArrayBuffer_T() :
            ConstantResource_T(Type::constantArrayBuffer) { }
    };

    LLAMA_CLASS_DECLARATION(ConstantArrayBuffer);

    LLAMA_API ConstantArrayBuffer createConstantArrayBuffer(Renderer renderer, size_t elementSize, uint32_t maxElementCount = 1000);
}