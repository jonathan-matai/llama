/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llentity.h
 Contains the Base Entity Class
*/

#pragma once

namespace llama
{
    class Entity
    {
    public:

        virtual void onServerTick(float delta) { }
        virtual void onClientTick(float delta) { }
    };
}