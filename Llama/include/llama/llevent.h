/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llevent.h
 Contains the Event and EventBus class
*/

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

    class Event
    {
    public:

        EventPriority m_priority;

        virtual ~Event() = default;

    protected:

        Event(EventPriority priority) :
            m_priority(priority) { }
    };

    class EventBus_T
    {
    public:

        template<typename EventType>
        void addDispatcher(std::function<bool(EventType*)> dispatcher);

        virtual void postEvent(Event* event) = 0;

    public:

        virtual ~EventBus_T() { }

    protected:

        std::unordered_map<std::type_index, std::list<void*>> m_dispatchers;
    };

    typedef std::shared_ptr<EventBus_T> EventBus;
    LLAMA_API EventBus createEventBus();

}

// ==== IMPLEMENTATION ====

template<typename EventType>
inline void llama::EventBus_T::addDispatcher(std::function<bool(EventType*)> dispatcher)
{
    auto r = m_dispatchers.find(typeid(EventType));

    if (r == m_dispatchers.end())
    {
        m_dispatchers.insert(std::make_pair(std::type_index(typeid(EventType)), std::list<void*>()));
        r = m_dispatchers.find(typeid(EventType));
    }

    void* function = reinterpret_cast<void*>(new std::function<bool(EventType*)>(dispatcher));
    r->second.push_back(function);
}