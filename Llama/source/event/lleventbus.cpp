#include "llcore.h"
#include "event/lleventbus.h"

namespace llama
{
    class EventBus_I : public EventBus_T
    {
    public:

    private:

        void addDispatcher(std::pair<EventTypeID, std::function<EventDispatchState(void*)>>&& dispatcher) override;

        void postEvent(std::unique_ptr<Event>&& event) override;

    private:

        std::vector<std::list<std::function<EventDispatchState(void*)>>> m_dispatchers;
        std::queue<std::unique_ptr<Event>> m_events;
    };
}


llama::EventBus llama::createEventBus()
{
    return std::make_shared<EventBus_I>();
}


void llama::EventBus_I::addDispatcher(std::pair<EventTypeID, std::function<EventDispatchState(void*)>>&& dispatcher)
{
    if (m_dispatchers.size() <= dispatcher.first)
        m_dispatchers.resize(dispatcher.first + 1);

    m_dispatchers[dispatcher.first].push_back(dispatcher.second);
}

void llama::EventBus_I::postEvent(std::unique_ptr<Event>&& event)
{
    // If the array is not that big, there is definetly no dispatcher and the event can be ignored
    if (event->m_type > m_dispatchers.size())
        return;

    if (event->m_priority == EventPriority::IMMEDIATE)
    {
        // Start at beginning of correspoinding dispatcher list
        auto a = m_dispatchers[event->m_type].begin();

        // Loop until the end of the list is reached
        while (a != m_dispatchers[event->m_type].end())
        {
            switch ((*a)(event.get()))
            {
            case EventDispatchState::DISPATCHED:

                return;

            case EventDispatchState::IGNORED:

                ++a;
                continue;

            case EventDispatchState::DISPATCHER_EXPIRED:

                a = m_dispatchers[event->m_type].erase(a);
            }
        }
    }
    else
    {
        m_events.push(std::move(event));
    }
}
