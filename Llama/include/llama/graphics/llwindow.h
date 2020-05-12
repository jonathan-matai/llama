/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llwindow.h
 Contains the Window Class
*/

#pragma once

struct GLFWwindow;

#include "../event/llevent.h"

namespace llama
{
    struct WindowDesc
    {
        WindowDesc() :
            width(800),
            height(450),
            title("Llama Application")
        { }

        uint32_t width;
        uint32_t height;
        std::string title;
    };

    class Window_T
    {
    public:

        virtual ~Window_T() = default;

        virtual GLFWwindow* getGLFWWindowHandle() const = 0;

    protected:

        Window_T(EventNode node) :
            m_node(node),
            m_tickDispatcher(node, this, &Window_T::onTick)
        { }

        EventNode m_node;
        EventDispatchFunction m_tickDispatcher;

        virtual EventDispatchState onTick(TickEvent* e) = 0;
    };

    typedef std::shared_ptr<Window_T> Window;

    LLAMA_API Window createWindow(EventNode node, const WindowDesc& description);
}