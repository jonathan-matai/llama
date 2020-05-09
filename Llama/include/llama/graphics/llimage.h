/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llimage.h
 Contains the Image Classes
*/

#pragma once

#include "llgraphics.h"

#include "llconstantset.h"

namespace llama
{
    class SampledImage_T : public ConstantResource_T
    {
    public:

        virtual ~SampledImage_T() = default;

    protected:

        SampledImage_T() :
            ConstantResource_T(Type::sampler)
        { }
    };

    using SampledImage = std::shared_ptr<SampledImage_T>;

    LLAMA_API SampledImage createSampledImage(GraphicsDevice device, std::string_view path);
}