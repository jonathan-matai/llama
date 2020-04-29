#ifndef LLPCH_H
#define LLPCH_H

#include "llcore.h"

#ifndef LLAMA_OS
    #define LLAMA_OS "Unknown"
#endif

#ifndef LLAMA_CONFIG
    #define LLAMA_CONFIG "Unknown"
#endif


#ifdef LLAMA_DEBUG
    #define LLAMA_DEBUG_ONLY(operation) operation
#else
    #define LLAMA_DEBUG_ONLY(operation)
#endif

#ifdef LLAMA_OS_WINDOWS
    // winsock2.h has to be included before Windows.h (included by vk_mem_alloc.hpp) to not produce any errors
    #include <winsock2.h>
#endif

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VULKAN_HPP_ASSERT(x)
#include <vk_mem_alloc.hpp>

#endif