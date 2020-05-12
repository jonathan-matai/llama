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
#include "../event/llevent.h"

namespace llama
{
    typedef std::shared_ptr<class Shader_T> Shader;

    class Renderer_T
    {
    public:

        virtual ~Renderer_T() = default;

        //virtual void tick() = 0;

        virtual void setShader(Shader shader, Shader shader2) = 0;


    protected:

        Renderer_T(EventNode node) :
            m_tickDispatcher(node, this, &Renderer_T::onTick)
        { }

        EventDispatchFunction m_tickDispatcher;

        virtual EventDispatchState onTick(TickEvent* e) = 0;
    };

    typedef std::shared_ptr<Renderer_T> Renderer;

    LLAMA_API Renderer createRenderer(EventNode node, GraphicsDevice device, Window context);
}