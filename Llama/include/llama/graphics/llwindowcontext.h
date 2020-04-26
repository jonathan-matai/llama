/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llwindowcontext.h
 Contains the Window Context
*/

#pragma once

#include "llwindow.h"
#include "llgraphics.h"

namespace llama
{
    class WindowContext_T
    {
    public:

        virtual ~WindowContext_T() = default;
    };

    typedef std::shared_ptr<WindowContext_T> WindowContext;

    LLAMA_API WindowContext createWindowContext(Window window, GraphicsDevice device);
}