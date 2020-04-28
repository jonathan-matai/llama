#include <llama.h>

#include <events.h>

#include <cstdio>

class Console
{
    llama::EventNode m_bus;
    llama::EventDispatchFunction m_func;

public:

    Console(llama::EventNode bus) :
        m_bus(bus),
        m_func(bus, this, &Console::print)
    {
    }

    llama::EventDispatchState print(PrintEvent* e)
    {
        printf("Recieved following PrintEvent: %s\n", e->m_message);
        return llama::EventDispatchState::DISPATCHED;
    }

    void run()
    {
        for (int i = 0; i < 4; ++i)
        {
            CalculatorEvent e;
            scanf("%d %c %d", &e.m_a, &e.m_operator, &e.m_b);
            m_bus->postEvent(CalculatorEvent(e));
        }
    }
};

int main()
{
    /*
    llama::EventBus bus = llama::createEventBus();

    llama::ClientSocket socket = llama::createClientSocket(bus);

    printf("Enter IP Version: ");
    int i;
    scanf("%d", &i);


    printf("Enter hostname of server: ");
    char buffer[128];
    scanf("%128s", buffer);

    socket->connectServer(buffer, "2020", i == 4 ? true : false);
    
    Console con(bus);

    con.run();

    bus->postEvent(llama::CloseApplicationEvent());
    */

    llama::Window window = llama::createWindow(llama::WindowDesc());

    llama::GraphicsDevice device = llama::createGraphicsDevice();
    
    llama::WindowContext context = llama::createWindowContext(window, device);

    llama::Shader shader = llama::createShader(context, "resources/shaders/triangle_static/triangle_static.vert.spv", "resources/shaders/triangle_static/triangle_static.frag.spv");

    llama::Renderer renderer = llama::createRenderer(context, shader);

    while (!window->shouldClose())
    {
        renderer->tick();
        window->tick();
    }
        

    return 0;
}