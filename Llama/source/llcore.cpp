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