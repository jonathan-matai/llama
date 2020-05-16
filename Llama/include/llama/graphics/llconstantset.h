/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llconstantset.h
 Contains Constant Set Class, a collection of Shader Inputs that are constant for one draw operation (e.g. Samplers, Constant Buffers)
*/

#pragma once

#include "llconstantresource.h"

namespace llama
{
    LLAMA_CLASS_DECLARATION(Shader);

    class ConstantSet_T
    {
    public:

        virtual ~ConstantSet_T() = default;

    protected:

        ConstantSet_T() { }
    };

    LLAMA_CLASS_DECLARATION(ConstantSet);

    LLAMA_API ConstantSet createConstantSet(Shader shader, uint32_t setIndex, std::initializer_list<ConstantResource> resources);
}