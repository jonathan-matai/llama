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
#include <cmath>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace llama
{
    constexpr float PI = 3.141592741f;


    template <typename compare_type>
    inline compare_type min(compare_type a, compare_type b);

    template <typename compare_type>
    inline compare_type max(compare_type a, compare_type b);

    template <typename compare_type>
    inline compare_type clamp(compare_type min, compare_type value, compare_type max);

    inline uint8_t hammingWeight(uint32_t bitfield);
    inline uint8_t hammingWeight(uint64_t bitfield);

    inline float round(float number);
    inline double round(double number);

    inline float radians(float degrees);
    inline float degrees(float radians);
 
}

#include "llvector.h"
#include "llmatrix.h"

#include "llmath.inl"