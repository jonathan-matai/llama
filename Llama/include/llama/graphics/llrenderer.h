/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llrenderer.h
 Contains the Shader Class
*/

#pragma once

#include "llgraphics.h"
#include "llwindow.h"

namespace llama
{
    typedef std::shared_ptr<class Shader_T> Shader;

    class Renderer_T
    {
    public:

        virtual ~Renderer_T() = default;

        virtual void tick() = 0;

        virtual void setShader(Shader shader) = 0;

    };

    typedef std::shared_ptr<Renderer_T> Renderer;

    LLAMA_API Renderer createRenderer(GraphicsDevice device, Window context);
}