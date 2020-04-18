/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llevent.h
 Contains the Event and EventBus class
*/

#pragma once

namespace llama
{
    enum class EventPriority
    {
        // Event gets processed immediately on the same thread that posts the message
        // Only suitable for events that are quick to dispatch
        IMMEDIATE,
        // Event gets processed asynchronously on the main thread preferrably the same tick
        // Only suitable for events that are quick to dispatch
        HIGH,
        // Event gets processed asynchronously on a seperate thread, doesn't block the game loop
        MEDIUM,
        // Event gets processed asynchronously on a seperate thread, with low priority
        LOW,
    };

    enum class InternalEventType
    {
        CLOSE_APPLICATION,
        BEGIN_USER_IMPLEMENTATION
    };

    typedef uint32_t EventTypeID;

    constexpr EventTypeID FIRST_EVENT_TYPE_ID = static_cast<uint32_t>(InternalEventType::BEGIN_USER_IMPLEMENTATION);

    /*
    EXAMPLE USAGE:

    enum class UserDefinedEventType
    {
        TYPE_1 = llama::FIRST_EVENT_TYPE_ID,
        TYPE_2,
        ...
        TYPE_X
    }

    llama::LAST_EVENT_TYPE_ID = UserDefinedEventType::TYPE_X;

    */

    class Event
    {
    public:

        EventTypeID m_type;
        EventPriority m_priority;

    protected:

        Event(EventTypeID type, EventPriority priority) :
            m_type(type),
            m_priority(priority) { }
    };

    class EventBus_T
    {
    public:

        // Adds a callback function for dispatching a certain type of event
        // @EventType:      The type of Event the callback function should recieve, must be a subclass of llama::Event
        // @DispatcherType: The type of the Dispatcher class, that contains the dispatching function, should ideally be a subclass of std::enable_shared_from_this
        // @dispatcher:     A weak_ptr to the Dispatcher, can be received with weak_from_this
        // @function:       The dispatching function
        //                  @param:     A pointer to the Event for dispatching
        //                  @return:    true, if Message shouldn't be dispatched by another dispatcher anymore
        template<typename EventType, typename DispatcherType>
        void addDispatcher(std::weak_ptr<DispatcherType> dispatcher, bool(DispatcherType::* function)(EventType*));


        // Posts an Event to the Message Bus for dispatching
        // @EventType:      The type of the event, must be a subclass of llama::Event
        // @event:          The event, that should be dispatched
        template<typename EventType>
        void postEvent(EventType&& event);

        // Posts an Event to the Message Bus for dispatching
        // @rawEvent:       A pointer to a subclass instance of llama::Event
        // @size:           The size of the subclass of llama::Event
        inline void postEvent(void* rawEvent, size_t size);

    private:

        enum class DispatchState
        {
            DISPATCHER_EXPIRED = -1,
            IGNORED = 0,
            DISPATCHED = 1
        };

        template<typename EventType, typename DispatcherType>
        static DispatchState dispatchIfNotExpired(void* event, std::weak_ptr<DispatcherType> dispatcher, bool(DispatcherType::* function)(EventType*));

        inline void dispatchEvent(void* event, EventTypeID type);

        std::vector<std::list<std::function<DispatchState(void*)>>> m_dispatchers;

        std::queue<Event*> m_events;
    };

    typedef std::shared_ptr<EventBus_T> EventBus;

    // Creates a new EventBus Object
    // @return:             The EventBus created
    EventBus createEventBus() { return std::make_shared<EventBus_T>(); }

}

#include "llevent.inl"