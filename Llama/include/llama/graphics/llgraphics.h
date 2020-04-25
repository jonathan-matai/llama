/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llgraphics.h
 Contains the Device Context
*/

#pragma once

namespace llama
{
    class GraphicsDevice_T
    {
    public:

        virtual ~GraphicsDevice_T() = default;
    };

    typedef std::shared_ptr<GraphicsDevice_T> GraphicsDevice;

    LLAMA_API GraphicsDevice createGraphicsDevice();
}