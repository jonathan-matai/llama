#include "llevent.h"
/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llevent.inl
 Implementation of the Event and EventBus class
*/


template<typename EventType, typename DispatcherType>
inline void llama::EventBus_T::addDispatcher(std::weak_ptr<DispatcherType> dispatcher, bool(DispatcherType::* function)(EventType*))
{
    // Retrieve EventTypeID of EventType 
    EventTypeID id = EventType::s_eventTypeID;

    if (m_dispatchers.size() <= id)
        m_dispatchers.resize(id + 1);

    m_dispatchers[id].push_back(std::bind(
        (DispatchState(*)(void*, std::weak_ptr<DispatcherType>, bool(DispatcherType::*)(EventType*))) &llama::EventBus_T::dispatchIfNotExpired,
            std::placeholders::_1, dispatcher, function));
}

template<typename EventType, typename DispatcherType>
inline llama::EventBus_T::DispatchState llama::EventBus_T::dispatchIfNotExpired(void* event, std::weak_ptr<DispatcherType> dispatcher, bool(DispatcherType::* function)(EventType*))
{
    if (dispatcher.expired())
        return DispatchState::DISPATCHER_EXPIRED;

    return (dispatcher.lock().get()->*function)(reinterpret_cast<EventType*>(event)) ? DispatchState::DISPATCHED : DispatchState::IGNORED;
}

template<typename EventType>
inline void llama::EventBus_T::postEvent(EventType&& event)
{
    //static_assert(std::is_base_of<llama::Event, EventType>::value, "EventType must derive from llama::Event");

    Event* e = reinterpret_cast<Event*>(&event);

    // If Event is Immediate begin processing
    if (e->m_priority == EventPriority::IMMEDIATE)
        dispatchEvent(&event, e->m_type);
    else
        m_events.push(reinterpret_cast<Event*>(new EventType(event)));
}

inline void llama::EventBus_T::postEvent(void* rawEvent, size_t maxSize)
{
    Event* event = reinterpret_cast<Event*>(rawEvent);
    EventTypeID type = event->m_type;
    EventPriority prio = event->m_priority;

    // If Event is Immediate begin processing
    if (prio == EventPriority::IMMEDIATE)
        dispatchEvent(event, type);
    else
    {
        Event* a = reinterpret_cast<Event*>(::operator new(maxSize));
        memcpy(a, rawEvent, maxSize);
        //memcpy_s(a, maxSize, rawEvent, maxSize);
        m_events.push(a);
    }
}

inline void llama::EventBus_T::dispatchEvent(void* event, EventTypeID type)
{
    // If the array is not that big, there is definetly no dispatcher and the event can be ignored
    if (type > m_dispatchers.size())
        return;

    // Start at beginning of correspoinding dispatcher list
    auto a = m_dispatchers[type].begin();

    // Loop until the end of the list is reached
    while (a != m_dispatchers[type].end())
    {
        switch ((*a)(event))
        {
        case DispatchState::DISPATCHED:

            return;

        case DispatchState::IGNORED:

            ++a;
            continue;

        case DispatchState::DISPATCHER_EXPIRED:

            a = m_dispatchers[type].erase(a);
        }
    }
}
