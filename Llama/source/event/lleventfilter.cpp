#include "llpch.h"
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
    RawDispatchFunction f = { this, std::bind(&EventNode_T::forwardEvent, m_childNode, std::placeholders::_1) };
    RawDispatchFunction g = { this, std::bind(&EventNode_T::forwardEvent, m_parentNode, std::placeholders::_1) };


    m_dispatchFunctions.push_back(EventDispatchFunction(m_parentNode, this, &EventFilter_T::downlinkCallback, f));
    m_dispatchFunctions.push_back(EventDispatchFunction(m_childNode, this, &EventFilter_T::uplinkCallback, g));
}

void llama::EventFilter_T::handleFilterRule(FilterDirection direction, FilterRule filter)
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

void llama::EventFilter_T::addDispatchFunction(EventTypeID eventTypeID, const RawDispatchFunction& function)
{
    if (eventTypeID == InternalEventType::ANY_EVENT || (m_downlinkFilters.size() > eventTypeID && m_downlinkFilters[eventTypeID] != nullptr))
        m_dispatchFunctions.push_back(EventDispatchFunction(m_parentNode, this, &EventFilter_T::downlinkCallback, function));

    else if (m_downlinkFilterMode == EventFilterMode::BLACKLIST)
        m_parentNode->addDispatchFunction(eventTypeID, function); // Bridge filter
}

void llama::EventFilter_T::removeDispatchFunction(EventTypeID eventTypeID, void* dispatcherObject)
{
    if (eventTypeID == InternalEventType::ANY_EVENT || (m_downlinkFilters.size() > eventTypeID && m_downlinkFilters[eventTypeID] != nullptr))
        for (auto a = m_dispatchFunctions.rbegin(); a != m_dispatchFunctions.rend();)
            if (a->m_eventTypeID == eventTypeID && a->m_dispatcherObject == dispatcherObject)
                m_dispatchFunctions.erase(std::next(a).base());
            else
                ++a;

    else if (m_downlinkFilterMode == EventFilterMode::BLACKLIST)
        m_parentNode->removeDispatchFunction(eventTypeID, dispatcherObject); // Bridge filter
}

llama::EventDispatchState llama::EventFilter_T::downlinkCallback(Event* event, const RawDispatchFunction& dispatcher)
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

llama::EventDispatchState llama::EventFilter_T::uplinkCallback(Event* event, const RawDispatchFunction& dispatcher)
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

void llama::EventFilter_T::handleEvent(std::unique_ptr<Event>&& event)
{
    event->m_creator = this;

    if (m_uplinkFilters.size() > event->m_type && m_uplinkFilters[event->m_type] != nullptr)
    {
        FilterResult a = m_uplinkFilters[event->m_type](event.get());

        if (a == FilterResult::APPROVED || a == FilterResult::EDITED)
            m_parentNode->handleEvent(std::move(event));
    }
    else if (m_uplinkFilterMode == EventFilterMode::BLACKLIST)
        m_parentNode->handleEvent(std::move(event));
    else
        logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "An Event of Type %d was blocked because Filter is in WHITELIST mode and no filter rule has been found!", event->m_type);
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

    logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "An Event of Type %d was blocked because Filter is in WHITELIST mode and no filter rule has been found!", event->m_type);
    return EventDispatchState::IGNORED;
}
