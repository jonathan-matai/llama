/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > lleventfilter.h
 Contains the EventFilter class
*/

#pragma once

#include "llevent.h"

namespace llama
{
    // Describes how Events should be handled, if no filter rule exists
    enum class EventFilterMode
    {
        // Only Events that have a filter rule are allowed, all other elements are blocked
        WHITELIST,
        // Events that don't have a filter rule pass the filter unchecked
        BLACKLIST
    };

    class EventFilter_T : public EventNode_T
    {
        friend class EventNode_T;

    public:

        // Create an EventFilter connecting another Node and an Endpoint/Receiver
        // @parentNode          The Parent Node to connect to
        // @uplinkFilterMode    How events from the Endpoint to the Node should be handled if there is no filter rule
        // @downlinkFilterMode  How events from the Node to the Endpoint should be handled if there is no filter rule
        LLAMA_API EventFilter_T(EventNode parentNode, EventFilterMode uplinkFilterMode, EventFilterMode downlinkFilterMode);

        // Create an EventFilter connecting two Nodes (parent and child node)
        // @parentNode          The Parent Node to connect to
        // @childNode           The Child Node to connect ot
        // @uplinkFilterMode    How messages from the Child Node to the Parent Node should be handled if there is no filter rule
        // @downlinkFilterMode  How message from the Parent Node to the Child Node should be handled if there is no filter rule
        LLAMA_API EventFilter_T(EventNode parentNode, EventNode childNode,
                                EventFilterMode uplinkFilterMode, EventFilterMode downlinkFilterMode);

        // Post an event to the Node
        // SHOULD BE ONLY CALLED FROM THE ENDPOINT AND ONLY IN ENDPOINT-NODE MODE
        // @event           Post an event created by the makeEvent() helper function
        //                  (e.g. postEvent(makeEvent(ExampleEvent(...)))
        LLAMA_API void handleEvent(std::unique_ptr<Event>&& event) override;

        // Post an event to the Node, that gets dispatched immediatly and returns DispatchState
        // SHOULD BE ONLY CALLED FROM THE ENDPOINT AND ONLY IN ENDPOINT-NODE MODE
        // @event           The forwarded element
        LLAMA_API EventDispatchState forwardEvent(Event* event) override;

    protected:

        // Describes the direction in which the filter rule should be applied
        enum class FilterDirection
        {
            // From Endpoint to Node / From Child to Parent Node
            UPLINK,
            // From Node to Endpoint / From Parent to Child Node
            DOWNLINK,
            // In both directions
            BIDIRCETIONAL
        };

        // Result of the filter rule
        enum class FilterResult
        {
            // Message has been blocked by the filter
            BLOCKED,
            // Message has been blocked, but additional processing is happening
            PROCESSED,
            // Message was edited and can now pass the filter
            EDITED,
            // Message may pass the filter
            APPROVED,
        };

        

        // Add a new filter rule
        // @EventType           Must be a subclass of llama::Event
        // @EventFilterClass    Must be a subclass of llama::EventFilter_T
        // @function            The callback function for the filter rule
        template<typename EventType, typename EventFilterClass>
        inline void addFilterRule(FilterDirection direction, FilterResult(EventFilterClass::* function)(EventType*));

        
        

        


    private:

        struct FilterRule
        {
            EventTypeID filterType;
            std::function<FilterResult(Event*)> callback;
        };

        LLAMA_API void handleFilterRule(FilterDirection direction, FilterRule filter);

        LLAMA_API void addDispatchFunction(EventTypeID eventTypeID, const RawDispatchFunction& function) override;
        LLAMA_API void removeDispatchFunction(EventTypeID eventTypeID, void* dispatcherObject) override;

        // Internal callback functions
        EventDispatchState downlinkCallback(Event* event, const RawDispatchFunction& dispatcher);
        EventDispatchState uplinkCallback(Event* event, const RawDispatchFunction& dispatcher);

        // Internal helper method for running filter functions
        template<typename EventType, typename EventFilterClass>
        static inline FilterResult runFilterFunction(Event* event,
                                                     EventFilterClass* dispatcher,
                                                     FilterResult(EventFilterClass::* function)(EventType*));

        EventFilterMode m_uplinkFilterMode, m_downlinkFilterMode;
        std::vector<std::function<FilterResult(Event*)>> m_uplinkFilters;
        std::vector<std::function<FilterResult(Event*)>> m_downlinkFilters;

        std::list<EventDispatchFunction> m_dispatchFunctions;

        EventNode m_parentNode, m_childNode;
    };

    typedef std::shared_ptr<EventFilter_T> EventFilter;
    
}

// IMPLEMENTATION

template<typename EventType, typename EventFilterClass>
inline void llama::EventFilter_T::addFilterRule(FilterDirection direction, FilterResult(EventFilterClass::* function)(EventType*))
{
    static_assert(std::is_base_of<llama::EventFilter_T, EventFilterClass>::value, "EventFilterClass must derive from llama::EventFilter_T");
    static_assert(std::is_base_of<llama::Event, EventType>::value, "EventType must derive from llama::Event");

    handleFilterRule(direction,
                     {
                         EventType::s_eventTypeID,
                         std::bind((FilterResult(*)(Event*, EventFilterClass*, FilterResult(EventFilterClass::*)(EventType*)))
                                    & EventFilter_T::runFilterFunction,
                                    std::placeholders::_1, static_cast<EventFilterClass*>(this), function)
                     });
}

template<typename EventType, typename EventFilterClass>
inline llama::EventFilter_T::FilterResult llama::EventFilter_T::runFilterFunction(Event* event, EventFilterClass* dispatcher, FilterResult(EventFilterClass::* function)(EventType*))
{
    return (dispatcher->*function)(reinterpret_cast<EventType*>(event));
}
