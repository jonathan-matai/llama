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
    // Lists all engine related events
    enum InternalEventType
    {
        ANY_EVENT = 0,
        CLOSE_APPLICATION,
        TICK,
        BEGIN_USER_IMPLEMENTATION
    };

    // Can be an InternalEventType or a user-defined EventType
    typedef uint32_t EventTypeID;

    // User-definded EventType must start here to not overlap with internal events
    constexpr EventTypeID FIRST_EVENT_TYPE_ID = static_cast<uint32_t>(InternalEventType::BEGIN_USER_IMPLEMENTATION);

    enum class EventDispatchState
    {
        IGNORED = 0,
        PROCESSED,
        DISPATCHED
    };

    enum class EventFlags
    {
        LOCAL_ONLY = bit(1),
        ALLOW_DEFERRING = bit(2),
        HIGH_PRIORITY = bit(3)
    };

    class Event
    {
    public:

        EventTypeID m_type;
        Flags<EventFlags> m_flags;
        void* m_creator;
        size_t m_size;

        static const EventTypeID s_eventTypeID = InternalEventType::ANY_EVENT;

    protected:

        Event(EventTypeID type, Flags<EventFlags> flags, size_t size, void* creator = nullptr) :
            m_type(type),
            m_flags(flags),
            m_creator(creator), 
            m_size(size) { }
    };


    class CloseApplicationEvent : public Event
    {
    public:

        CloseApplicationEvent() :
            Event(InternalEventType::CLOSE_APPLICATION, { }, sizeof(CloseApplicationEvent))
        { }

        static const EventTypeID s_eventTypeID = InternalEventType::CLOSE_APPLICATION;
    };

    class TickEvent : public Event
    {
    public:

        TickEvent(uint32_t tickrateIndex, float delta, uint64_t tickIndex) :
            Event(InternalEventType::TICK, { }, sizeof(TickEvent)),
            m_tickrateIndex(tickrateIndex),
            m_deltaTime(delta),
            m_tickIndex(tickIndex)
        {}

        uint32_t m_tickrateIndex;
        float m_deltaTime;
        uint64_t m_tickIndex;

        static const EventTypeID s_eventTypeID = InternalEventType::TICK;
    };

    struct EventDispatcher
    {
        EventTypeID eventType;
        std::function<EventDispatchState(Event*)> callback;

        EventDispatcher(EventTypeID type, const std::function<EventDispatchState(Event*)>& function) :
            eventType(type), callback(function) { }
    };
    

    class EventNode_T
    {
        friend class EventDispatchFunction;
        friend class EventFilter_T;
        friend class ClientSocket_I;
        friend class ServerSocket_I;

    public:

        // Post an event to the Node, that gets dispatched immediatly and returns DispatchState
        // @event           The forwarded element
        virtual EventDispatchState forwardEvent(Event* event) = 0;

        // Post an event to the Node
        // @EventType       Must be a subclass of llama::Event
        // @event           Post an event to the node
        template<typename EventType>
        inline void postEvent(EventType&& event);

    protected:

        virtual inline void handleEvent(std::unique_ptr<Event>&& event);

        struct RawDispatchFunction
        {
            void* dispatcherObject;
            std::function<EventDispatchState(Event*)> callback;
        };

        // Gets called upon creating an EventDispatchFunction
        // Add a new dispatch function to the Node
        // @eventTypeID         The Type of Event that should get dispatched, can be InternalEventType::ANY_EVENT
        // @function            The dispatching function, see definiton of RawDispatchFunction
        virtual void addDispatchFunction(EventTypeID eventTypeID, const RawDispatchFunction& function) = 0;

        // Gets called upon destroying an EventDispatchFunction
        // Remove all Dispatch Functions belonging to dispatcherObject with the type EventTypeID
        // @eventTypeID         The Type of Event that the function dispatched
        // @dispatcherObject    The object of the function
        virtual void removeDispatchFunction(EventTypeID eventTypeID, void* dispatcherObject) = 0;
    };

    typedef std::shared_ptr<EventNode_T> EventNode;

    class EventFilter_T;

    class EventDispatchFunction
    {
        friend class EventFilter_T;
    public:

        template<typename EventType, typename DispatcherClass>
        EventDispatchFunction(EventNode node,
                              DispatcherClass* object,
                              EventDispatchState(DispatcherClass::* function)(EventType*));

        inline ~EventDispatchFunction();

        inline EventDispatchFunction(EventDispatchFunction&& other);
        EventDispatchFunction(const EventDispatchFunction&) = delete;

    private:

        template<typename DispatcherClass>
        EventDispatchFunction(EventNode node,
                              EventFilter_T* object,
                              EventDispatchState(DispatcherClass::* function)(Event*, const EventNode_T::RawDispatchFunction&),
                              const EventNode_T::RawDispatchFunction& rawFunction);

        template<typename EventType, typename DispatcherClass>
        static inline EventDispatchState dispatchEvent(Event* event,
                                                       DispatcherClass* dispatcher,
                                                       EventDispatchState(DispatcherClass::* function)(EventType*));

        EventNode m_node;
        void* m_dispatcherObject;
        EventTypeID m_eventTypeID;
    };
}

#include "llevent.inl"