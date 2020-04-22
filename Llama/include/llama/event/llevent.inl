/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llevent.inl
 Implementation of the Event and EventBus class
*/


template<typename EventType>
std::unique_ptr<llama::Event> llama::makeEvent(EventType&& event)
{
    static_assert(std::is_base_of<llama::Event, EventType>::value, "EventType must derive from llama::Event");
    return std::unique_ptr<Event>(static_cast<Event*>(std::make_unique<EventType>(event).release()));
}

template<typename EventType, typename DispatcherClass>
llama::EventDispatcher llama::makeDispatcher(std::weak_ptr<DispatcherClass> dispatcher,
                                      EventDispatchState(DispatcherClass::* function)(EventType*))
{
    return llama::EventDispatcher(EventType::s_eventTypeID, 
                                  std::bind((EventDispatchState(*)(Event*, std::weak_ptr<DispatcherClass>, EventDispatchState(DispatcherClass::*)(EventType*)))
                                            &llama::EventNode_T::dispatchIfNotExpired,
                                            std::placeholders::_1, dispatcher, function));
}

template<typename EventType, typename DispatcherType>
inline llama::EventDispatchState llama::EventNode_T::dispatchIfNotExpired(Event* event, 
                                                                          std::weak_ptr<DispatcherType> dispatcher, 
                                                                          EventDispatchState(DispatcherType::* function)(EventType*))
{
    if (dispatcher.expired())
        return EventDispatchState::DISPATCHER_EXPIRED;

    std::shared_ptr<DispatcherType> d = dispatcher.lock();

    if (d.get() == event->m_creator)
        return EventDispatchState::IGNORED;

    return (d.get()->*function)(reinterpret_cast<EventType*>(event));
}

inline void llama::EventNode_T::postEvent(std::unique_ptr<Event>&& event)
{
    forwardEvent(event.get());
}