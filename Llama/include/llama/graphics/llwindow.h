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

        virtual bool shouldClose() = 0;

        virtual void tick() = 0;

        virtual GLFWwindow* getGLFWWindowHandle() const = 0;
    };

    typedef std::shared_ptr<Window_T> Window;

    LLAMA_API Window createWindow(const WindowDesc& description);
}