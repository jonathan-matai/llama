#include <llama.h>

#include <events.h>

class NetworkFilter : public llama::EventFilter_T
{
public:
    NetworkFilter(llama::EventBus bus) :
        EventFilter_T(bus, llama::EventFilterMode::WHITELIST, llama::EventFilterMode::BLACKLIST)
    {
        addFilterRule(FilterDirection::UPLINK, &NetworkFilter::filterCalculatorEvent);
        addFilterRule(FilterDirection::UPLINK, &NetworkFilter::filterCloseApplicationEvent);
    }

    FilterResult filterCalculatorEvent(CalculatorEvent* e)
    {
        if ((e->m_operator == '/' || e->m_operator == ':') && e->m_b == 0)
        {
            llama::logfile()->print(llama::Colors::RED, "Event blocked! Division by 0!\n");
            return FilterResult::BLOCKED;
        }

        if (e->m_operator == '/' ||
            e->m_operator == ':' ||
            e->m_operator == '*' ||
            e->m_operator == 'x' ||
            e->m_operator == '+' ||
            e->m_operator == '-')
            return FilterResult::APPROVED;

        llama::logfile()->print(llama::Colors::RED, "Event blocked! Illegal operator!\n");
        return FilterResult::BLOCKED;
    }

    FilterResult filterCloseApplicationEvent(llama::CloseApplicationEvent* e)
    {
        return FilterResult::APPROVED;
    }
};

class Calculator
{
    llama::EventNode m_bus;
    llama::EventDispatchFunction m_func;

public:

    Calculator(llama::EventNode bus) :
        m_bus(bus),
        m_func(bus, this, &Calculator::calculate)
    { }

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

int main()
{
    llama::EventBus bus = llama::createEventBus();

    std::shared_ptr<NetworkFilter> filter = std::make_shared<NetworkFilter>(bus);

    Calculator calc(bus);

    llama::ServerSocket socket = llama::createServerSocket(filter, 2020, false);

    return 0;
}