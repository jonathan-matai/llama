#include <string>

#include "events.h"

struct CalculatorEvent : public llama::Event
{
    int m_a, m_b;
    char m_operator;
    
    static const llama::EventTypeID s_eventTypeID = CALCULATOR_EVENT;

    CalculatorEvent(int a, char op, int b) :
        llama::Event(CALCULATOR_EVENT, llama::EventPriority::IMMEDIATE),
        m_a(a),
        m_b(b),
        m_operator(op) { }

    CalculatorEvent() :
        llama::Event(CALCULATOR_EVENT, llama::EventPriority::IMMEDIATE) { }
};

struct PrintEvent : public llama::Event
{
    std::string m_message;

    static const llama::EventTypeID s_eventTypeID = PRINT_EVENT;

    PrintEvent(std::string_view message) : 
        llama::Event(PRINT_EVENT, llama::EventPriority::IMMEDIATE),
        m_message(message) { }
};

class Calculator : public std::enable_shared_from_this<Calculator>
{
    llama::EventNode m_bus;

public:

    Calculator(llama::EventNode bus) :
        m_bus(bus)
    {        
    }

    void addToDefaultBus()
    {
        m_bus->addDispatcher(makeDispatcher(weak_from_this(), &Calculator::calculate));
    }

    llama::EventDispatchState calculate(CalculatorEvent* e)
    {
        int result = 0;

        switch (e->m_operator)
        {
        case '+':
            result = e->m_a + e->m_b;
            break;
        case '-':
            result = e->m_a - e->m_b;
            break;
        case '*':
        case 'x':
            result = e->m_a * e->m_b;
            break;
        case '/':
        case ':':
            result = e->m_a / e->m_b;
            break;
        }

        m_bus->postEvent(makeEvent(PrintEvent(std::string("The number is ") + std::to_string(result))));

        return llama::EventDispatchState::DISPATCHED;
    }
};

class Console : public std::enable_shared_from_this<Console>
{
    llama::EventNode m_bus;

public:

    Console(llama::EventNode bus) :
        m_bus(bus)
    {
    }

    void addToDefaultBus()
    {
        m_bus->addDispatcher(makeDispatcher(weak_from_this(), &Console::print));
    }

    llama::EventDispatchState print(PrintEvent* e)
    {
        printf("Recieved following PrintEvent: %s\n", e->m_message.c_str());
        return llama::EventDispatchState::DISPATCHED;
    }

    void run()
    {
        while (true)
        {
            CalculatorEvent e;
            scanf("%d %c %d", &e.m_a, &e.m_operator, &e.m_b);
            m_bus->postEvent(makeEvent(CalculatorEvent(e)));
        }
    }
};