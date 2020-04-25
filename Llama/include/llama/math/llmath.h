/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llmath.h
 The Llama Math Library
*/

#pragma once

#include <immintrin.h>

namespace llama
{
    inline uint8_t hammingWeight(uint32_t bitfield);
    inline uint8_t hammingWeight(uint64_t bitfield);
}

#include "llmath.inl"