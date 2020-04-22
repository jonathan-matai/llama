#include "llcore.h"
#include "event/lleventfilter.h"

llama::EventFilter_T::EventFilter_T(EventNode parentNode, EventFilterMode uplinkFilterMode, EventFilterMode downlinkFilterMode) :
    m_downlinkFilterMode(downlinkFilterMode),
    m_uplinkFilterMode(uplinkFilterMode),
    m_parentNode(parentNode),
    m_childNode(nullptr)
{

}

llama::EventFilter_T::EventFilter_T(EventNode parentNode, EventNode childNode, EventFilterMode uplinkFilterMode, EventFilterMode downlinkFilterMode) :
    m_downlinkFilterMode(downlinkFilterMode),
    m_uplinkFilterMode(uplinkFilterMode),
    m_parentNode(parentNode),
    m_childNode(childNode)
{
    EventDispatcher downlinkDispatcher(InternalEventType::ANY_EVENT, std::bind(&EventNode_T::forwardEvent, m_childNode, std::placeholders::_1));
    EventDispatcher uplinkDispatcher(InternalEventType::ANY_EVENT, std::bind(&EventNode_T::forwardEvent, m_parentNode, std::placeholders::_1));

    m_parentNode->addDispatcher({ InternalEventType::ANY_EVENT, std::bind((EventDispatchState(EventFilter_T::*)(void*, EventDispatcher&))
                                                                      &EventFilter_T::downlinkCallback,
                                                                      this, std::placeholders::_1, downlinkDispatcher) });

    m_childNode->addDispatcher({ InternalEventType::ANY_EVENT, std::bind((EventDispatchState(EventFilter_T::*)(void*, EventDispatcher&))
                                                                      &EventFilter_T::uplinkCallback,
                                                                      this, std::placeholders::_1, uplinkDispatcher) });
}

void llama::EventFilter_T::addFilterRule(FilterDirection direction, FilterRule filter)
{
    if (direction == FilterDirection::DOWNLINK || direction == FilterDirection::BIDIRCETIONAL)
    {
        if (m_downlinkFilters.size() <= filter.filterType)
            m_downlinkFilters.resize(filter.filterType + 1);

        m_downlinkFilters[filter.filterType] = filter.callback;
    }
        
    if (direction == FilterDirection::UPLINK || direction == FilterDirection::BIDIRCETIONAL)
    {
        if (m_uplinkFilters.size() <= filter.filterType)
            m_uplinkFilters.resize(filter.filterType + 1);

        m_uplinkFilters[filter.filterType] = filter.callback;
    }
}

llama::EventDispatchState llama::EventFilter_T::downlinkCallback(Event* event, EventDispatcher& dispatcher)
{
    if (event->m_creator == this)
        return EventDispatchState::IGNORED;

    void* oldCreator = event->m_creator;
    event->m_creator = this;

    if (m_downlinkFilters.size() > event->m_type && m_downlinkFilters[event->m_type] != nullptr)
    {
        FilterResult result = m_downlinkFilters[event->m_type](event);

        if (result == FilterResult::APPROVED || result == FilterResult::EDITED)
        {
            EventDispatchState state = dispatcher.callback(event);
            event->m_creator = oldCreator;
            return state;
        }

        event->m_creator = oldCreator;
        return EventDispatchState::IGNORED;
    }
    else if(m_downlinkFilterMode == EventFilterMode::BLACKLIST)
    {
        EventDispatchState state = dispatcher.callback(event);
        event->m_creator = oldCreator;
        return state;
    }

    event->m_creator = oldCreator;
    return EventDispatchState::IGNORED;
}

llama::EventDispatchState llama::EventFilter_T::uplinkCallback(Event* event, EventDispatcher& dispatcher)
{
    if (event->m_creator == this)
        return EventDispatchState::IGNORED;

    void* oldCreator = event->m_creator;
    event->m_creator = this;

    if (m_uplinkFilters.size() > event->m_type && m_uplinkFilters[event->m_type] != nullptr)
    {
        FilterResult result = m_uplinkFilters[event->m_type](event);

        if (result == FilterResult::APPROVED || result == FilterResult::EDITED)
        {
            EventDispatchState state = dispatcher.callback(event);
            event->m_creator = oldCreator;
            return state;
        }

        event->m_creator = oldCreator;
        return EventDispatchState::IGNORED;
    }
    else if (m_uplinkFilterMode == EventFilterMode::BLACKLIST)
    {
        EventDispatchState state = dispatcher.callback(event);
        event->m_creator = oldCreator;
        return state;
    }

    event->m_creator = oldCreator;
    return EventDispatchState::IGNORED;
}

void llama::EventFilter_T::addDispatcher(EventDispatcher&& dispatcher)
{
    if (m_downlinkFilters[dispatcher.eventType] != nullptr)
        m_parentNode->addDispatcher({ dispatcher.eventType, std::bind((EventDispatchState(EventFilter_T::*)(void*, EventDispatcher&)) 
                                                                      &EventFilter_T::downlinkCallback, 
                                                                      this, std::placeholders::_1, dispatcher) });
    else if(m_downlinkFilterMode == EventFilterMode::BLACKLIST)
        m_parentNode->addDispatcher(std::move(dispatcher));
}

void llama::EventFilter_T::postEvent(std::unique_ptr<Event>&& event)
{
    event->m_creator = this;

    if (m_uplinkFilters.size() > event->m_type && m_uplinkFilters[event->m_type] != nullptr)
    {
        FilterResult a = m_uplinkFilters[event->m_type](event.get());

        if(a == FilterResult::APPROVED || a == FilterResult::EDITED)
            m_parentNode->postEvent(std::move(event));
    }  
    else if (m_uplinkFilterMode == EventFilterMode::BLACKLIST)
        m_parentNode->postEvent(std::move(event));
}

llama::EventDispatchState llama::EventFilter_T::forwardEvent(Event* event)
{
    event->m_creator = this;

    if (m_uplinkFilters.size() > event->m_type && m_uplinkFilters[event->m_type] != nullptr)
    {
        FilterResult a = m_uplinkFilters[event->m_type](event);

        if (a == FilterResult::APPROVED || a == FilterResult::EDITED)
            return m_parentNode->forwardEvent(event);
    }
    else if (m_uplinkFilterMode == EventFilterMode::BLACKLIST)
        return m_parentNode->forwardEvent(event);

    return EventDispatchState::IGNORED;
}
