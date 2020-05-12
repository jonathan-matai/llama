#include "llpch.h"
#include "graphics/llwindow.h"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

namespace llama
{
    class Window_I : public Window_T
    {
    public:

        Window_I(EventNode node, const WindowDesc& description);
        ~Window_I() override;

    private:

        static void debugCallback(int error, const char* description);

        EventDispatchState onTick(TickEvent* e) override;

        GLFWwindow* getGLFWWindowHandle() const override;

        GLFWwindow* m_window;
    };
}

llama::Window_I::Window_I(EventNode node, const WindowDesc& description) :
    Window_T(node)
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
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void llama::Window_I::debugCallback(int error, const char* description)
{
    logfile()->print(Colors::RED, "GLFW Error %x: %s", error, description);
}

llama::EventDispatchState llama::Window_I::onTick(TickEvent* e)
{
    if (e->m_tickrateIndex != 0)
        return EventDispatchState::IGNORED;

    glfwPollEvents();

    if (glfwWindowShouldClose(m_window) != 0)
        m_node->postEvent(CloseApplicationEvent());

    return EventDispatchState::PROCESSED;
}

GLFWwindow* llama::Window_I::getGLFWWindowHandle() const
{
    return m_window;
}

llama::Window llama::createWindow(EventNode node, const WindowDesc& description)
{
    return std::make_shared<Window_I>(node, description);
}
