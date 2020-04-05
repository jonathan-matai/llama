#include <llama.h>

#include <typeindex>
#include <cstdio>

class RandomEvent : public llama::Event
{
public:

    RandomEvent(const char* msg) :
        Event(llama::EventPriority::IMMEDIATE),
        m_msg(msg) { }

    const char* m_msg;
};

class NumberEvent : public llama::Event
{
public:

    NumberEvent(int number) : 
        Event(llama::EventPriority::IMMEDIATE),
        m_number(number) { }

    int m_number;
};

bool dispatchRandomEvent(RandomEvent* e)
{
    printf("The string is %s\n", e->m_msg);
    return false;
}

bool dispatchNumberEvent(NumberEvent* e)
{
    printf("The number is %d\n", e->m_number);
    return false;
}

int main()
{
    llama::EventBus bus = llama::createEventBus();
    bus->addDispatcher(std::function(dispatchRandomEvent));
    bus->addDispatcher(std::function(dispatchNumberEvent));

    bus->postEvent(new NumberEvent(666));
    bus->postEvent(new RandomEvent("Hello World!"));

    return 0;
}