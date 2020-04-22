#include "llcore.h"
#include "event/lleventbus.h"

namespace llama
{
    class EventBus_I : public EventBus_T
    {
    public:

    private:

        void addDispatcher(EventDispatcher&& dispatcher) override;

        void postEvent(std::unique_ptr<Event>&& event) override;

        EventDispatchState forwardEvent(Event* event) override;

    private:

        std::vector<std::list<std::function<EventDispatchState(Event*)>>> m_dispatchers;
        std::queue<std::unique_ptr<Event>> m_events;
    };
}


llama::EventBus llama::createEventBus()
{
    return std::make_shared<EventBus_I>();
}


void llama::EventBus_I::addDispatcher(EventDispatcher&& dispatcher)
{
    if (m_dispatchers.size() <= dispatcher.eventType)
        m_dispatchers.resize(dispatcher.eventType + 1);

    m_dispatchers[dispatcher.eventType].push_back(dispatcher.callback);

    LLAMA_DEBUG_ONLY(logfile()->print(Colors::GREY, "Added disptacher to EventBus %p for EventTypeId %d ",
                     this, dispatcher.eventType));
}

void llama::EventBus_I::postEvent(std::unique_ptr<Event>&& event)
{
    if (event->m_priority == EventPriority::IMMEDIATE)
    {
        forwardEvent(event.get());
    }
    else
    {
        // If the array is not that big, there is definetly no dispatcher and the event can be ignored
        if (event->m_type > m_dispatchers.size())
            return;

        m_events.push(std::move(event));
    }
}

llama::EventDispatchState llama::EventBus_I::forwardEvent(Event* event)
{
    EventDispatchState state = EventDispatchState::IGNORED;

    // If the array is not that big, there is definetly no dispatcher and the event can be ignored
    if (event->m_type > m_dispatchers.size())
        return state;


    // Start at beginning of correspoinding dispatcher list
    auto a = m_dispatchers[InternalEventType::ANY_EVENT].begin();

    // Loop until the end of the list is reached
    while (a != m_dispatchers[InternalEventType::ANY_EVENT].end())
    {
        switch ((*a)(event))
        {
        case EventDispatchState::DISPATCHED:

            return EventDispatchState::DISPATCHED;

        case EventDispatchState::IGNORED:

            ++a;
            continue;

        case EventDispatchState::PROCESSED:

            state = EventDispatchState::PROCESSED;
            ++a;
            continue;

        case EventDispatchState::DISPATCHER_EXPIRED:

            a = m_dispatchers[event->m_type].erase(a);
        }
    }

    // Start at beginning of correspoinding dispatcher list
    a = m_dispatchers[event->m_type].begin();

    // Loop until the end of the list is reached
    while (a != m_dispatchers[event->m_type].end())
    {
        switch ((*a)(event))
        {
        case EventDispatchState::DISPATCHED:

            return EventDispatchState::DISPATCHED;

        case EventDispatchState::IGNORED:

            ++a;
            continue;

        case EventDispatchState::PROCESSED:

            state = EventDispatchState::PROCESSED;
            ++a;
            continue;

        case EventDispatchState::DISPATCHER_EXPIRED:

            a = m_dispatchers[event->m_type].erase(a);
        }
    }

    return state;
}
