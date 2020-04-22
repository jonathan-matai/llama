/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > lleventbus.h
 Contains the EventBus class
*/

#pragma once

#include "llevent.h"

namespace llama
{
    class EventBus_T : public EventNode_T
    {
    public:

        // Add a dispatcher to the Node
        // @dispatcher      A dispatcher function created by the makeDispatcher() helper function
        //                  (e.g. addDispatcher(makeDispatcher(weak_from_this(), &Receiver::method)))
        virtual void addDispatcher(EventDispatcher&& dispatcher) override = 0;

        // Post an event to the Node
        // @event           Post an event created by the makeEvent() helper function
        //                  (e.g. postEvent(makeEvent(ExampleEvent(...)))
        virtual void postEvent(std::unique_ptr<Event>&& event) override = 0;

        // Post an event to the Node, that gets dispatched immediatly and returns DispatchState
        // @event           Post an event created by the makeEvent() helper function
        //                  (e.g. postEvent(makeEvent(ExampleEvent(...)))
        virtual EventDispatchState forwardEvent(Event* event) override = 0;

    };

    typedef std::shared_ptr<EventBus_T> EventBus;

    LLAMA_API EventBus createEventBus();
}