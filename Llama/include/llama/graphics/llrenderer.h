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
#include "../event/llevent.h"
#include "../entity/llentitymanager.h"

namespace llama
{
    LLAMA_CLASS_DECLARATION(Shader);
    LLAMA_CLASS_DECLARATION(Window);

    class Renderer_T
    {
    public:

        virtual ~Renderer_T() = default;

        virtual void addEntityManager(EntityManager manager) = 0;

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