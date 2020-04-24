#include <llama.h>

enum Events
{
    CALCULATOR_EVENT = llama::FIRST_EVENT_TYPE_ID,
    PRINT_EVENT
};

struct CalculatorEvent : public llama::Event
{
    int m_a, m_b;
    char m_operator;

    static const llama::EventTypeID s_eventTypeID = CALCULATOR_EVENT;

    CalculatorEvent(int a, char op, int b) :
        llama::Event(CALCULATOR_EVENT, llama::EventPriority::IMMEDIATE, sizeof(CalculatorEvent)),
        m_a(a),
        m_b(b),
        m_operator(op) { }

    CalculatorEvent() :
        llama::Event(CALCULATOR_EVENT, llama::EventPriority::IMMEDIATE, sizeof(CalculatorEvent)) { }
};

struct PrintEvent : public llama::Event
{
    char m_message[64];

    static const llama::EventTypeID s_eventTypeID = PRINT_EVENT;

    PrintEvent(std::string_view message) :
        llama::Event(PRINT_EVENT, llama::EventPriority::IMMEDIATE, sizeof(PrintEvent))
    {
        memcpy(m_message, message.data(), message.size() < 64 ? message.size() : 64);
        m_message[message.size()] = '\0';
    }
};