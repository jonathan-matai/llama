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

#include "llwindowcontext.h"
#include "llshader.h"

namespace llama
{
    class Renderer_T
    {
    public:

        virtual ~Renderer_T() = default;

        virtual void tick() = 0;

    };

    typedef std::shared_ptr<Renderer_T> Renderer;

    LLAMA_API Renderer createRenderer(WindowContext context, Shader shader);
}