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

        virtual void handleEvent(std::unique_ptr<Event>&& event) override = 0;

        virtual EventDispatchState forwardEvent(Event* event) override = 0;

    };

    typedef std::shared_ptr<EventBus_T> EventBus;

    LLAMA_API EventBus createEventBus();
}