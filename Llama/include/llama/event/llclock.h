/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llclock.h
 Clock that send tick events to the message bus
*/

#pragma once

#include "lleventbus.h"
#include "../logging/lltime.h"

namespace llama
{
    class Clock_T
    {
    public:

        virtual ~Clock_T() = default;

        // Runs the clock, which continuously send Tick events to the message bus
        // Doesn't return until a Stop Event is received
        virtual void run() = 0;
    };

    using Clock = std::shared_ptr<Clock_T>;


    // Creates a clock object
    // @bus:        The Event Bus the Clock will send events to and receive events from
    // @tickrates:  You can e.g. create a server tick at 60 tps and a client tick with unlimited tickrate
    //              A tickrate of 0 means, the Clock will tick as fast as possible
    LLAMA_API Clock createClock(EventBus bus, std::initializer_list<float> tickrates);
}