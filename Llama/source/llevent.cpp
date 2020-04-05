#include "llcore.h"
#include "llevent.h"

namespace llama
{
    class EventBus_I : public EventBus_T
    {
    public:

        EventBus_I() { }
        ~EventBus_I() { }

    private:

        void postEvent(Event* event) override;

    private:

        std::queue<Event*> m_events;
    };

    void EventBus_I::postEvent(Event* event)
    {
        if (event->m_priority == EventPriority::IMMEDIATE)
        {
            auto r = m_dispatchers.find(typeid(*event));

            if (r != m_dispatchers.end())
                for(const auto a : r->second)
                    if((*reinterpret_cast<std::function<bool(void*)>*>(a))((void*)event))
                        break;
        }
        else
        {
            m_events.push(event);
        }
    }
}

llama::EventBus llama::createEventBus()
{
    return std::make_shared<EventBus_I>();
}
