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

    // Lists all engine related events
    enum InternalEventType
    {
        ANY_EVENT = 0,
        CLOSE_APPLICATION,
        BEGIN_USER_IMPLEMENTATION
    };

    // Can be an InternalEventType or a user-defined EventType
    typedef uint32_t EventTypeID;

    // User-definded EventType must start here to not overlap with internal events
    constexpr EventTypeID FIRST_EVENT_TYPE_ID = static_cast<uint32_t>(InternalEventType::BEGIN_USER_IMPLEMENTATION);

    enum class EventDispatchState
    {
        DISPATCHER_EXPIRED = -1,
        IGNORED = 0,
        PROCESSED,
        DISPATCHED
    };

    class Event
    {
    public:

        EventTypeID m_type;
        EventPriority m_priority;
        void* m_creator;

    protected:

        Event(EventTypeID type, EventPriority priority, void* creator = nullptr) : 
            m_type(type),
            m_priority(priority),
            m_creator(creator) { }
    };

    struct EventDispatcher
    {
        EventTypeID eventType;
        std::function<EventDispatchState(Event*)> callback;

        EventDispatcher(EventTypeID type, const std::function<EventDispatchState(Event*)>& function) :
            eventType(type), callback(function) { }
    };

    // Helper Function to pass an Event to an Event Node
    // @EventType:          Must be a sublcass of llama::Event
    // @event:              A newly constructed Event (e.g. makeEvent(ExampleEvent(...))
    // @return:             A Event unique_ptr to pass to EventNode_T::postEvent()
    template<typename EventType>
    inline std::unique_ptr<Event> makeEvent(EventType&& event);

    // Helper Function to pass a dispatcher Function to an Event Node
    // @EventType:          Must be a subclass of llama::Event
    // @DispatcherClass:    The class the Dispatcher Function is located in
    // @dispatcher:         A weak_ptr to the object containing the dispatcher funtion (can be retrieved with weak_from_this())
    // @function:           The dispatching function
    //                      @param:     A pointer to the Event for dispatching
    //                      @return:    The State of Dispatching, view Declaration of EventDispatchState for more information
    template<typename EventType, typename DispatcherClass>
    EventDispatcher makeDispatcher(std::weak_ptr<DispatcherClass> dispatcher,
                                   EventDispatchState(DispatcherClass::* function)(EventType*));
    

    class EventNode_T
    {
        

    public:

        // Add a dispatcher to the Node
        // @dispatcher      A dispatcher function created by the makeDispatcher() helper function
        //                  (e.g. addDispatcher(makeDispatcher(weak_from_this(), &Receiver::method)))
        virtual void addDispatcher(EventDispatcher&& dispatcher) = 0;

        // Post an event to the Node
        // @event           Post an event created by the makeEvent() helper function
        //                  (e.g. postEvent(makeEvent(ExampleEvent(...)))
        virtual inline void postEvent(std::unique_ptr<Event>&& event);

        // Post an event to the Node, that gets dispatched immediatly and returns DispatchState
        // @event           The forwarded element
        virtual EventDispatchState forwardEvent(Event* event) = 0;

    private:

        // Friend declaration
        template<typename EventType, typename DispatcherClass>
        friend inline EventDispatcher makeDispatcher(std::weak_ptr<DispatcherClass> dispatcher,
                                                     EventDispatchState(DispatcherClass::* function)(EventType*));

        // Internal helper method for dispatching Events
        template<typename EventType, typename DispatcherType>
        static inline EventDispatchState dispatchIfNotExpired(Event* event, 
                                                       std::weak_ptr<DispatcherType> dispatcher, 
                                                       EventDispatchState(DispatcherType::* function)(EventType*));
    };

    typedef std::shared_ptr<EventNode_T> EventNode;
}

#include "llevent.inl"