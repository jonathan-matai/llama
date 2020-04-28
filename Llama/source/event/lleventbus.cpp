#include "llpch.h"
#include "event/lleventbus.h"

namespace llama
{
    class EventBus_I : public EventBus_T
    {
    public:

    private:

        void handleEvent(std::unique_ptr<Event>&& event) override;

        EventDispatchState forwardEvent(Event* event) override;

    private:

        void addDispatchFunction(EventTypeID eventTypeID, const RawDispatchFunction& function) override;
        void removeDispatchFunction(EventTypeID eventTypeID, void* dispatcherObject) override;

        std::vector<std::list<RawDispatchFunction>> m_dispatchers;
        std::queue<std::unique_ptr<Event>> m_events;
    };
}


llama::EventBus llama::createEventBus()
{
    return std::make_shared<EventBus_I>();
}


void llama::EventBus_I::handleEvent(std::unique_ptr<Event>&& event)
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

    // Loop until the end of the list is reached
    for (const auto& a : m_dispatchers[InternalEventType::ANY_EVENT])
    {
        switch ((a.callback)(event))
        {
        case EventDispatchState::DISPATCHED:

            return EventDispatchState::DISPATCHED;

        case EventDispatchState::PROCESSED:

            state = EventDispatchState::PROCESSED;
        }
    }

    // If the array is not that big, there is definetly no dispatcher and the event can be ignored
    if (event->m_type >= m_dispatchers.size())
        return state;

    // Loop until the end of the list is reached
    for (const auto& a : m_dispatchers[event->m_type])
    {
        switch ((a.callback)(event))
        {
        case EventDispatchState::DISPATCHED:

            return EventDispatchState::DISPATCHED;

        case EventDispatchState::PROCESSED:

            state = EventDispatchState::PROCESSED;
        }
    }

    return state;
}

void llama::EventBus_I::addDispatchFunction(EventTypeID eventTypeID, const RawDispatchFunction& function)
{
    if (m_dispatchers.size() <= eventTypeID)
        m_dispatchers.resize(eventTypeID + 1);

    m_dispatchers[eventTypeID].push_back(function);

    LLAMA_DEBUG_ONLY(logfile()->print(Colors::GREY, "Added disptacher to EventBus %p for EventTypeId %d ",
                                      this, eventTypeID));
}

void llama::EventBus_I::removeDispatchFunction(EventTypeID eventTypeID, void* dispatcherObject)
{
    for (auto a = m_dispatchers[eventTypeID].begin(); a != m_dispatchers[eventTypeID].end();)
        if (a->dispatcherObject == dispatcherObject)
            a = m_dispatchers[eventTypeID].erase(a);
        else
            ++a;
}
