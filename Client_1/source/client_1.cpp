#include <llama.h>

#include <events.h>
#include <cstdio>

#include "coolllama.h"

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

    llama::EventBus bus = llama::createEventBus();

    llama::EntityManager manager = llama::createEntityManager(bus);

    llama::Clock clock = llama::createClock(bus, { 0.0f, 60.0f });

    llama::WindowDesc desc;
    desc.width = 800;
    desc.height = 800;

    llama::Window window = llama::createWindow(bus, desc);

    llama::GraphicsDevice device = llama::createGraphicsDevice();

    llama::Renderer renderer = llama::createRenderer(bus, device, window);

    CoolLlama::createStaticResources(device, renderer);

    manager->addEntity(CoolLlama(1, llama::float2(-.5f, 0.1f)));
    manager->addEntity(CoolLlama(2, llama::float2(0.0f, 0.2f)));
    manager->addEntity(CoolLlama(3, llama::float2(0.5f, -.1f)));

    renderer->addEntityManager(manager);

    clock->run();

    return 0;
}