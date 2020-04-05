/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llcore.h
 The precompiled header of the Llama Game Library
*/


#ifndef LLCORE_H
#define LLCORE_H

#ifdef LLAMA_OS_WINDOWS

    #ifdef LLAMA_BUILD
        #define LLAMA_API __declspec(dllexport)
    #else
        #define LLAMA_API __declspec(dllimport)
    #endif

#else
    #define LLAMA_API
#endif

// std::function
#include <functional>

// std::unordered_map
#include <unordered_map>

// std::type_index, std::type_info
#include <typeindex>

// std::queue, std::dequeue, std::vector
#include <queue>

namespace llama
{
    LLAMA_API void helloWorld();
}

#endif