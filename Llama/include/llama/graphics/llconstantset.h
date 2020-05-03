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


#include "llshader.h"

namespace llama
{
    class ConstantResource_T
    {
    public:

        enum class Type
        {
            constantBuffer,
            constantArrayBuffer,
            storageBuffer,
            storageArrayBuffer,
            sampler
        };

        ConstantResource_T(Type resourceType) :
            m_resourceType(resourceType)
        { }

        inline Type getResourceType() const { return m_resourceType; }

    protected:

        Type m_resourceType;
    };

    typedef std::shared_ptr<ConstantResource_T> ConstantResource;

    class ConstantSet_T
    {
    public:

        virtual ~ConstantSet_T() = default;

    protected:

        ConstantSet_T() { }
    };

    typedef std::shared_ptr<ConstantSet_T> ConstantSet;

    LLAMA_API ConstantSet createConstantSet(Shader shader, uint32_t setIndex, std::initializer_list<ConstantResource> resources);
}