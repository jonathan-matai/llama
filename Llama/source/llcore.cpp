#include "llcore.h"

#include <cstdio>

size_t llama::stringLength(std::string_view string)
{
    size_t length = 0;

    for (auto a = string.cbegin(); a != string.cend(); ++a) // Loop through all characters
        if ((*a & 0xc0) != 0x80) // Skip additional bytes from multibyte character
            ++length;

    return length;
}


#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE