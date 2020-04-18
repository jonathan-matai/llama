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
    llama::EventBus m_bus;

public:

    Calculator(llama::EventBus bus) :
        m_bus(bus)
    {        
    }

    void addToDefaultBus()
    {
        m_bus->addDispatcher(weak_from_this(), &Calculator::calculate);
    }

    bool calculate(CalculatorEvent* e)
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

        m_bus->postEvent(PrintEvent(std::string("The number is ") + std::to_string(result)));
        return true;
    }
};

class Console : public std::enable_shared_from_this<Console>
{
    llama::EventBus m_bus;

public:

    Console(llama::EventBus bus) :
        m_bus(bus)
    {
    }

    void addToDefaultBus()
    {
        m_bus->addDispatcher(weak_from_this(), &Console::print);
    }

    bool print(PrintEvent* e)
    {
        printf("Recieved following PrintEvent: %s\n", e->m_message.c_str());
        return true;
    }

    void run()
    {
        while (true)
        {
            CalculatorEvent e;
            scanf_s("%d %c %d", &e.m_a, &e.m_operator, 1, &e.m_b);
            m_bus->postEvent(CalculatorEvent(e));
        }
    }
};