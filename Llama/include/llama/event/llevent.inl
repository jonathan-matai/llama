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


template<typename EventType>
inline void llama::EventNode_T::postEvent(EventType&& event)
{
    static_assert(std::is_base_of<llama::Event, EventType>::value, "EventType must derive from llama::Event");
    handleEvent(std::unique_ptr<Event>(static_cast<Event*>(std::make_unique<EventType>(event).release())));
}

inline void llama::EventNode_T::handleEvent(std::unique_ptr<Event>&& event)
{
    forwardEvent(event.get());
}

template<typename EventType, typename DispatcherClass>
inline llama::EventDispatchFunction::EventDispatchFunction(EventNode node,
                                                           DispatcherClass* object,
                                                           EventDispatchState(DispatcherClass::* function)(EventType*)) :
    m_node(node),
    m_dispatcherObject(object)
{
    static_assert(std::is_base_of<llama::Event, EventType>::value, "EventType must derive from llama::Event");
    m_eventTypeID = EventType::s_eventTypeID;

    m_node->addDispatchFunction(m_eventTypeID,
                                {
                                    object,
                                    std::bind((EventDispatchState(*)(Event*, DispatcherClass*, EventDispatchState(DispatcherClass::*)(EventType*)))
                                              &EventDispatchFunction::dispatchEvent,
                                              std::placeholders::_1, object, function)
                                });
}

template<typename DispatcherClass>
inline llama::EventDispatchFunction::EventDispatchFunction(EventNode node, 
                                                           EventFilter_T* object, 
                                                           EventDispatchState(DispatcherClass::* function)(Event*, const EventNode_T::RawDispatchFunction&), 
                                                           const EventNode_T::RawDispatchFunction& rawFunction) :
    m_node(node),
    m_dispatcherObject(object)
{
    m_eventTypeID = Event::s_eventTypeID;

    m_node->addDispatchFunction(m_eventTypeID,
                                {
                                    object,
                                    std::bind(function,
                                              reinterpret_cast<DispatcherClass*>(object), std::placeholders::_1, rawFunction)
                                });
}

template<typename EventType, typename DispatcherClass>
inline llama::EventDispatchState llama::EventDispatchFunction::dispatchEvent(Event* event, 
                                                                             DispatcherClass* dispatcher, 
                                                                             EventDispatchState(DispatcherClass::* function)(EventType*))
{
    if (event->m_creator == dispatcher)
        return EventDispatchState::IGNORED;

    return (dispatcher->*function)(static_cast<EventType*>(event));
}

inline llama::EventDispatchFunction::~EventDispatchFunction()
{
    if(m_node != nullptr)
        m_node->removeDispatchFunction(m_eventTypeID, m_dispatcherObject);
}

inline llama::EventDispatchFunction::EventDispatchFunction(EventDispatchFunction&& other) :
    m_dispatcherObject(other.m_dispatcherObject),
    m_eventTypeID(other.m_eventTypeID),
    m_node(other.m_node)
{
    other.m_node = nullptr;
}
