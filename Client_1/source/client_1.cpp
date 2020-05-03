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

    llama::WindowDesc desc;
    desc.width = 800;
    desc.height = 800;

    llama::Window window = llama::createWindow(desc);

    llama::GraphicsDevice device = llama::createGraphicsDevice();

    llama::Renderer renderer = llama::createRenderer(device, window);

    llama::Shader shader = llama::createShader(renderer, "resources/shaders/triangles/triangles.json");

    renderer->setShader(shader);

    while (!window->shouldClose())
    {
        renderer->tick();
        window->tick();
    }
        

    return 0;
}