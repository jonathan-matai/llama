#include <string>

#include "events.h"



class Calculator : public std::enable_shared_from_this<Calculator>
{
    llama::EventNode m_bus;
    llama::EventDispatchFunction m_func;

public:

    Calculator(llama::EventNode bus) :
        m_bus(bus),
        m_func(bus, this, &Calculator::calculate)
    {        
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

        m_bus->postEvent(PrintEvent(std::string("The number is ") + std::to_string(result)));

        return llama::EventDispatchState::DISPATCHED;
    }
};

class Console : public std::enable_shared_from_this<Console>
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
        printf("Recieved following PrintEvent: %s\n", e->m_message.c_str());
        return llama::EventDispatchState::DISPATCHED;
    }

    void run()
    {
        for(int i = 0; i < 4; ++i)
        {
            CalculatorEvent e;
            scanf("%d %c %d", &e.m_a, &e.m_operator, &e.m_b);
            m_bus->postEvent(CalculatorEvent(e));
        }
    }
};

class CalculatorFilter : public llama::EventFilter_T
{
public:

    CalculatorFilter(llama::EventNode bus) :
        EventFilter_T(bus, llama::EventFilterMode::BLACKLIST, llama::EventFilterMode::BLACKLIST)
    {
        addFilterRule(FilterDirection::DOWNLINK, &CalculatorFilter::filterCalculatorEvent);
    }

    FilterResult filterCalculatorEvent(CalculatorEvent* e)
    {
        if ((e->m_operator == '/' || e->m_operator == ':') && e->m_b == 0)
        {
            printf("Event blocked! Division by 0!\n");
            return FilterResult::BLOCKED;
        }

        return FilterResult::APPROVED;
    }
};

class CalculatorBusFilter : public llama::EventFilter_T
{
public:

    CalculatorBusFilter(llama::EventNode bus, llama::EventNode bus2) :
        EventFilter_T(bus, bus2, llama::EventFilterMode::BLACKLIST, llama::EventFilterMode::BLACKLIST)
    {
        addFilterRule(FilterDirection::BIDIRCETIONAL, &CalculatorFilter::filterCalculatorEvent);
    }

    FilterResult filterCalculatorEvent(CalculatorEvent* e)
    {
        if ((e->m_operator == '/' || e->m_operator == ':') && e->m_b == 0)
        {
            printf("Event blocked! Division by 0!\n");
            return FilterResult::BLOCKED;
        }

        return FilterResult::APPROVED;
    }
};