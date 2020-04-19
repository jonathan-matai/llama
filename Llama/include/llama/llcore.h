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

// std::string, std::string_view
#include <string>

// std::thread, std::mutex, std::lock_guard
#include <mutex>

// std::is_base_of
#include <type_traits>

// std::array
#include <array>

// std::queue
#include <queue>

// std::filesystem::...
#include <filesystem>

typedef uint8_t byte;

// llama::Color
#include "logging/llcolor.h"

// llama::Timestamp
#include "logging/lltime.h"

// llama::Logfile, llama::Table
#include "logging/lllogfile.h"

namespace llama
{
    // Returns the length of a string
    //
    // @string: UTF8-encoded string
    // @return: Character count of string (not the actual byte size!)
    LLAMA_API size_t stringLength(std::string_view string);

    constexpr size_t bit(uint8_t bit) { return size_t(1) << bit; }
}

#endif