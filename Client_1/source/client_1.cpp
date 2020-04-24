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


    /*
    llama::EventBus bus = llama::createEventBus();

    llama::logfile()->print(llama::Colors::WHITE, "Created event bus!");

    std::shared_ptr<CalculatorFilter> filter = std::make_shared<CalculatorFilter>(bus);

    Calculator calc(filter);
    Console con(bus);

    con.run();
    */
    
    /*
    llama::EventBus calculatorBus = llama::createEventBus();
    llama::EventBus printerbus = llama::createEventBus();

    llama::logfile()->print(llama::Colors::WHITE, "Created event busses!");

    std::shared_ptr<CalculatorBusFilter> filter = std::make_shared<CalculatorBusFilter>(calculatorBus, printerbus);

    std::shared_ptr<Calculator> calc = std::make_shared<Calculator>(calculatorBus);
    std::shared_ptr<Console> con = std::make_shared<Console>(printerbus);

    con->run();
    */
    return 0;
}