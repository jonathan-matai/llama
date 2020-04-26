#include "llcore.h"
#include "graphics/llwindow.h"

#include <GLFW/glfw3.h>

namespace llama
{
    class Window_I : public Window_T
    {
    public:

        Window_I(const WindowDesc& description);
        ~Window_I() override;

    private:

        static void debugCallback(int error, const char* description);

        GLFWwindow* m_window;
    };
}

llama::Window_I::Window_I(const WindowDesc& description)
{
    Timestamp start;

    if (!glfwInit())
    {
        logfile()->print(Colors::RED, LLAMA_DEBUG_INFO, "glfwInit() failed! Cannot initialize GLFW!");
        return;
    }

    glfwSetErrorCallback(&Window_I::debugCallback);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(description.width, description.height, description.title.c_str(), nullptr, nullptr);

    logfile()->print(Colors::GREEN, "Created Window! (%s)", duration(start, Timestamp()).c_str());
}

llama::Window_I::~Window_I()
{
    while (!glfwWindowShouldClose(m_window))
        glfwPollEvents();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void llama::Window_I::debugCallback(int error, const char* description)
{
    logfile()->print(Colors::RED, "GLFW Error %x: %s", error, description);
}

llama::Window llama::createWindow(const WindowDesc& description)
{
    return std::make_shared<Window_I>(description);
}
