/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llmatrix.h
 The Matrix Classes of the Llama Math Library
*/

#pragma once

namespace llama
{
    union float2x2
    {
    public:

        struct
        {
            struct alignas(16) { float a1, b1; };
            struct alignas(16) { float a2, b2; };
        };


        inline float2x2();
        inline float2x2(float a1, float b1,
                        float a2, float b2);

        inline float2x2 operator+(const float2x2& matrix);
        inline float2x2 operator-(const float2x2& matrix);
        inline float2x2 operator*(const float2x2& matrix);
        inline float2x2 operator*(float value);

        inline float2x2 operator+=(const float2x2& matrix);
        inline float2x2 operator-=(const float2x2& matrix);
        inline float2x2 operator*=(const float2x2& matrix);
        inline float2x2 operator*=(float value);

    private:

        inline float2x2(__m128 _1, __m128 _2);

        __m128 m_sse[2];
    };

    union float3x3
    {
    public:

        struct
        {
            struct alignas(16) { float a1, b1, c1; };
            struct alignas(16) { float a2, b2, c2; };
            struct alignas(16) { float a3, b3, c3; };
        };

        inline float3x3();
        inline float3x3(float a1, float b1, float c1,
                        float a2, float b2, float c2,
                        float a3, float b3, float c3);

        inline float3x3 operator+(const float3x3& matrix);
        inline float3x3 operator-(const float3x3& matrix);
        inline float3x3 operator*(const float3x3& matrix);
        inline float3x3 operator*(float value);

        inline float3x3 operator+=(const float3x3& matrix);
        inline float3x3 operator-=(const float3x3& matrix);
        inline float3x3 operator*=(const float3x3& matrix);
        inline float3x3 operator*=(float value);

    private:

        inline float3x3(__m128 _1, __m128 _2, __m128 _3);

        __m128 m_sse[3];
    };

    inline float3x3 rotation2D(float radians);
    inline float3x3 scaling2D(float scaleX, float scaleY);
    inline float3x3 translation2D(const float2& translation);

    union float4x4
    {
    public:

        struct
        {
            float a1, b1, c1, d1;
            float a2, b2, c2, d2;
            float a3, b3, c3, d3;
            float a4, b4, c4, d4;
        };

        inline float4x4();
        inline float4x4(float a1, float b1, float c1, float d1,
                        float a2, float b2, float c2, float d2,
                        float a3, float b3, float c3, float d3,
                        float a4, float b4, float c4, float d4);

        inline float4x4 operator+(const float4x4& matrix);
        inline float4x4 operator-(const float4x4& matrix);
        inline float4x4 operator*(const float4x4& matrix);
        inline float4x4 operator*(float value);

        inline float4x4 operator+=(const float4x4& matrix);
        inline float4x4 operator-=(const float4x4& matrix);
        inline float4x4 operator*=(const float4x4& matrix);
        inline float4x4 operator*=(float value);

    private:

        inline float4x4(__m128 _1, __m128 _2, __m128 _3, __m128 _4);

        __m128 m_sse[4];
    };

    inline float4x4 scaling3D(const float3& scale);
    inline float4x4 translation3D(const float3& translation);
    inline float4x4 rotationX3D(float radians);
    inline float4x4 rotationY3D(float radians);
    inline float4x4 rotationZ3D(float radians);
    inline float4x4 view(const float3& position, const float3& rotationRadians);
    inline float4x4 projection(float aspectRatio, float fovRadians, float nearPlane, float farPlane);
}

#include "llmatrix.inl"