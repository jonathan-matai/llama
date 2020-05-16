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

    llama::Group group(0);
    group.addEntity(llama::Entity(1));
    group.addEntity(llama::Entity(2));



    llama::EventBus bus = llama::createEventBus();

    llama::Clock clock = llama::createClock(bus, { 0.0f, 60.0f });

    llama::WindowDesc desc;
    desc.width = 800;
    desc.height = 800;

    llama::Window window = llama::createWindow(bus, desc);

    llama::GraphicsDevice device = llama::createGraphicsDevice();

    llama::Renderer renderer = llama::createRenderer(bus, device, window);

    llama::Shader shader1 = llama::createShader(renderer, "resources/shaders/triangles/triangles.json");
    llama::Shader shader2 = llama::createShader(renderer, "resources/shaders/image2d/image2d.json");

    renderer->setShader(shader1, shader2);

    clock->run();

    return 0;
}