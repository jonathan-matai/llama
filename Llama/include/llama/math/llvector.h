/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llvector.h
 The Vector Classes of the Llama Math Library
*/

#pragma once

#include <immintrin.h>

namespace llama
{

    union float2;

    inline float2 operator*(float value, const float2& vec);
    inline float length(const float2& vec);
    inline float2 normalize(const float2& vec);
    inline float dot(const float2& a, const float2& b);
    inline float cross(const float2& a, const float2& b);

    union float2
    {
    public:

        struct { float x, y; };

        inline float2();
        inline float2(const float2& other);
        inline float2(float value);
        inline float2(float x, float y);

        inline float2 operator+(const float2& vec);
        inline float2 operator-(const float2& vec);
        inline float2 operator*(const float2& vec);
        inline float2 operator*(float value);
        inline float2 operator/(const float2& vec);
        inline float2 operator/(float value);

        inline float2& operator+=(const float2& vec);
        inline float2& operator-=(const float2& vec);
        inline float2& operator*=(const float2& vec);
        inline float2& operator*=(float value);
        inline float2& operator/=(const float2& vec);
        inline float2& operator/=(float value);

        friend inline float2 operator*(float value, const float2& vec);
        friend inline float length(const float2& vec);
        friend inline float2 normalize(const float2& vec);
        friend inline float dot(const float2& a, const float2& b);
        friend inline float cross(const float2& a, const float2& b);

    private:

        inline float2(__m128 other);

        __m128 m_sse;

    };

    union float3;

    inline float3 operator*(float value, const float3& vec);
    inline float length(const float3& vec);
    inline float3 normalize(const float3& vec);
    inline float dot(const float3& a, const float3& b);
    inline float3 cross(const float3& a, const float3& b);

    union float3
    {
    public:

        struct { float x, y, z; };

        inline float3();
        inline float3(const float3& other);
        inline float3(float value);
        inline float3(float x, float y, float z);

        inline float3 operator+(const float3& vec);
        inline float3 operator-(const float3& vec);
        inline float3 operator*(const float3& vec);
        inline float3 operator*(float value);
        inline float3 operator/(const float3& vec);
        inline float3 operator/(float value);

        inline float3& operator+=(const float3& vec);
        inline float3& operator-=(const float3& vec);
        inline float3& operator*=(const float3& vec);
        inline float3& operator*=(float value);
        inline float3& operator/=(const float3& vec);
        inline float3& operator/=(float value);

        friend inline float3 operator*(float value, const float3& vec);
        friend inline float length(const float3& vec);
        friend inline float3 normalize(const float3& vec);
        friend inline float dot(const float3& a, const float3& b);
        friend inline float3 cross(const float3& a, const float3& b);

    private:

        inline float3(__m128 other);

        __m128 m_sse;

    };

    union float4;

    inline float4 operator*(float value, const float4& vec);
    inline float length(const float4& vec);
    inline float4 normalize(const float4& vec);
    inline float dot(const float4& a, const float4& b);

    union float4
    {
    public:

        struct { float x, y, z, w; };

        inline float4();
        inline float4(const float4& other);
        inline float4(float value);
        inline float4(float x, float y, float z, float w);

        inline float4 operator+(const float4& vec);
        inline float4 operator-(const float4& vec);
        inline float4 operator*(const float4& vec);
        inline float4 operator*(float value);
        inline float4 operator/(const float4& vec);
        inline float4 operator/(float value);

        inline float4& operator+=(const float4& vec);
        inline float4& operator-=(const float4& vec);
        inline float4& operator*=(const float4& vec);
        inline float4& operator*=(float value);
        inline float4& operator/=(const float4& vec);
        inline float4& operator/=(float value);

        friend inline float4 operator*(float value, const float4& vec);
        friend inline float length(const float4& vec);
        friend inline float4 normalize(const float4& vec);
        friend inline float dot(const float4& a, const float4& b);

    private:

        inline float4(__m128 other);

        __m128 m_sse;

    };
}

#include "llvector.inl"