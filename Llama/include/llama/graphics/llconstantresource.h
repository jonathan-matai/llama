/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llconstantresource.h
 Contains the Constant Resource Interface, all Constant Resource Type classes must derive from this
*/

#pragma once

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

    LLAMA_CLASS_DECLARATION(ConstantResource);
}