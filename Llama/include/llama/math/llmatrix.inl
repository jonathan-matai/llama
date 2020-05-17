/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llmatrix.h
 The Implementation of the Matrix Classes of the Llama Math Library
*/

llama::float2x2::float2x2()
{
    // Create Identity Matrix
    m_sse[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    m_sse[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
}

llama::float2x2::float2x2(float _a1, float _b1, 
                          float _a2, float _b2)
{
    m_sse[0] = _mm_set_ps(0.0f, 0.0f, _b1, _a1);
    m_sse[1] = _mm_set_ps(0.0f, 0.0f, _b2, _a2);
}

llama::float2x2::float2x2(__m128 _1, __m128 _2)
{
    m_sse[0] = _1;
    m_sse[1] = _2;
}

llama::float2x2 llama::float2x2::operator+(const float2x2& matrix)
{
    return float2x2(_mm_add_ps(m_sse[0], matrix.m_sse[0]), _mm_add_ps(m_sse[1], matrix.m_sse[1]));
}

llama::float2x2 llama::float2x2::operator-(const float2x2& matrix)
{
    return float2x2(_mm_sub_ps(m_sse[0], matrix.m_sse[0]), _mm_sub_ps(m_sse[1], matrix.m_sse[1]));
}

llama::float2x2 llama::float2x2::operator*(const float2x2& matrix)
{
    return float2x2(
        _mm_add_ps(
            _mm_mul_ps(_mm_permute_ps(m_sse[0], 0b11100000), matrix.m_sse[0]),
            _mm_mul_ps(_mm_permute_ps(m_sse[0], 0b11100101), matrix.m_sse[1])
        ),
        _mm_add_ps(
            _mm_mul_ps(_mm_permute_ps(m_sse[1], 0b11100000), matrix.m_sse[0]),
            _mm_mul_ps(_mm_permute_ps(m_sse[1], 0b11100101), matrix.m_sse[1])
        )
    );
}

llama::float2x2 llama::float2x2::operator*(float value)
{
    return float2x2(_mm_mul_ps(m_sse[0], _mm_set_ps1(value)), _mm_mul_ps(m_sse[1], _mm_set_ps1(value)));
}

llama::float2x2 llama::float2x2::operator+=(const float2x2& matrix)
{
    return *this = float2x2(_mm_add_ps(m_sse[0], matrix.m_sse[0]), _mm_add_ps(m_sse[1], matrix.m_sse[1]));
}

llama::float2x2 llama::float2x2::operator-=(const float2x2& matrix)
{
    return *this = float2x2(_mm_sub_ps(m_sse[0], matrix.m_sse[0]), _mm_sub_ps(m_sse[1], matrix.m_sse[1]));
}

llama::float2x2 llama::float2x2::operator*=(const float2x2& matrix)
{
    return *this = float2x2(
        _mm_add_ps(
            _mm_mul_ps(_mm_permute_ps(m_sse[0], 0b11100000), matrix.m_sse[0]),
            _mm_mul_ps(_mm_permute_ps(m_sse[0], 0b11100101), matrix.m_sse[1])
        ),
        _mm_add_ps(
            _mm_mul_ps(_mm_permute_ps(m_sse[1], 0b11100000), matrix.m_sse[0]),
            _mm_mul_ps(_mm_permute_ps(m_sse[1], 0b11100101), matrix.m_sse[1])
        )
    );
}

llama::float2x2 llama::float2x2::operator*=(float value)
{
    return *this = float2x2(_mm_mul_ps(m_sse[0], _mm_set_ps1(value)), _mm_add_ps(m_sse[1], _mm_set_ps1(value)));
}

llama::float3x3::float3x3()
{
    // Create Identity Matrix
    m_sse[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    m_sse[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    m_sse[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
}

llama::float3x3::float3x3(float _a1, float _b1, float _c1, float _a2, float _b2, float _c2, float _a3, float _b3, float _c3)
{
    m_sse[0] = _mm_set_ps(0.0f, _c1, _b1, _a1);
    m_sse[1] = _mm_set_ps(0.0f, _c2, _b2, _a2);
    m_sse[2] = _mm_set_ps(0.0f, _c3, _b3, _a3);
}

llama::float3x3::float3x3(__m128 _1, __m128 _2, __m128 _3)
{
    m_sse[0] = _1;
    m_sse[1] = _2;
    m_sse[2] = _3;
}

llama::float3x3 llama::float3x3::operator+(const float3x3& matrix)
{
    return float3x3(_mm_add_ps(m_sse[0], matrix.m_sse[0]),
                    _mm_add_ps(m_sse[1], matrix.m_sse[1]),
                    _mm_add_ps(m_sse[2], matrix.m_sse[2]));
}

llama::float3x3 llama::float3x3::operator-(const float3x3& matrix)
{
    return float3x3(_mm_sub_ps(m_sse[0], matrix.m_sse[0]),
                    _mm_sub_ps(m_sse[1], matrix.m_sse[1]),
                    _mm_sub_ps(m_sse[2], matrix.m_sse[2]));
}

llama::float3x3 llama::float3x3::operator*(const float3x3& matrix)
{
    return
    {
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a1), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b1), matrix.m_sse[1])),
            _mm_mul_ps(_mm_set_ps1(c1), matrix.m_sse[2])
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a2), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b2), matrix.m_sse[1])),
            _mm_mul_ps(_mm_set_ps1(c2), matrix.m_sse[2])
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a3), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b3), matrix.m_sse[1])),
            _mm_mul_ps(_mm_set_ps1(c3), matrix.m_sse[2])
        )
    };
}

llama::float3x3 llama::float3x3::operator*(float value)
{
    return float3x3(_mm_mul_ps(m_sse[0], _mm_set_ps1(value)),
                    _mm_mul_ps(m_sse[1], _mm_set_ps1(value)),
                    _mm_mul_ps(m_sse[2], _mm_set_ps1(value)));
}

llama::float3x3 llama::float3x3::operator+=(const float3x3& matrix)
{
    return *this = float3x3(_mm_add_ps(m_sse[0], matrix.m_sse[0]),
                            _mm_add_ps(m_sse[1], matrix.m_sse[1]),
                            _mm_add_ps(m_sse[2], matrix.m_sse[2]));
}

llama::float3x3 llama::float3x3::operator-=(const float3x3& matrix)
{
    return *this = float3x3(_mm_sub_ps(m_sse[0], matrix.m_sse[0]),
                            _mm_sub_ps(m_sse[1], matrix.m_sse[1]),
                            _mm_sub_ps(m_sse[2], matrix.m_sse[2]));
}

llama::float3x3 llama::float3x3::operator*=(const float3x3& matrix)
{
    return *this =
    {
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a1), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b1), matrix.m_sse[1])),
            _mm_mul_ps(_mm_set_ps1(c1), matrix.m_sse[2])
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a2), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b2), matrix.m_sse[1])),
            _mm_mul_ps(_mm_set_ps1(c2), matrix.m_sse[2])
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a3), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b3), matrix.m_sse[1])),
            _mm_mul_ps(_mm_set_ps1(c3), matrix.m_sse[2])
        )
    };
}

llama::float3x3 llama::float3x3::operator*=(float value)
{
    return *this = float3x3(_mm_mul_ps(m_sse[0], _mm_set_ps1(value)),
                            _mm_mul_ps(m_sse[1], _mm_set_ps1(value)),
                            _mm_mul_ps(m_sse[2], _mm_set_ps1(value)));
}

llama::float3x3 llama::rotation2D(float radians)
{
    return float3x3(::cosf(radians), ::sinf(radians), 0.0f,
                    -::sinf(radians), ::cosf(radians), 0.0f,
                    0.0f, 0.0f, 1.0f);
}

llama::float3x3 llama::scaling2D(float scaleX, float scaleY)
{
    return float3x3(scaleX, 0.0f, 0.0f,
                    0.0f, scaleY, 0.0f,
                    0.0f, 0.0f, 1.0f);
}

llama::float3x3 llama::translation2D(const float2& translation)
{
    return float3x3(1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    translation.x, translation.y, 1.0f);
}


llama::float4x4::float4x4()
{
    // Create Identity Matrix
    m_sse[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    m_sse[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    m_sse[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    m_sse[2] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
}

llama::float4x4::float4x4(float a1, float b1, float c1, float d1,
                          float a2, float b2, float c2, float d2,
                          float a3, float b3, float c3, float d3,
                          float a4, float b4, float c4, float d4)
{
    m_sse[0] = _mm_set_ps(d1, c1, b1, a1);
    m_sse[1] = _mm_set_ps(d2, c2, b2, a2);
    m_sse[2] = _mm_set_ps(d3, c3, b3, a3);
    m_sse[3] = _mm_set_ps(d4, c4, b4, a4);
}

llama::float4x4::float4x4(__m128 _1, __m128 _2, __m128 _3, __m128 _4)
{
    m_sse[0] = _1;
    m_sse[1] = _2;
    m_sse[2] = _3;
    m_sse[3] = _4;
}

llama::float4x4 llama::float4x4::operator+(const float4x4& matrix)
{
    return float4x4(_mm_add_ps(m_sse[0], matrix.m_sse[0]),
                    _mm_add_ps(m_sse[1], matrix.m_sse[1]),
                    _mm_add_ps(m_sse[2], matrix.m_sse[2]),
                    _mm_add_ps(m_sse[3], matrix.m_sse[3]));
}

llama::float4x4 llama::float4x4::operator-(const float4x4& matrix)
{
    return float4x4(_mm_sub_ps(m_sse[0], matrix.m_sse[0]),
                    _mm_sub_ps(m_sse[1], matrix.m_sse[1]),
                    _mm_sub_ps(m_sse[2], matrix.m_sse[2]),
                    _mm_sub_ps(m_sse[3], matrix.m_sse[3]));
}

llama::float4x4 llama::float4x4::operator*(const float4x4& matrix)
{
    return
    {
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a1), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b1), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c1), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d1), matrix.m_sse[3]))
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a2), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b2), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c2), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d2), matrix.m_sse[3]))
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a3), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b3), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c3), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d3), matrix.m_sse[3]))
    ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a4), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b4), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c4), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d4), matrix.m_sse[3]))
        )
    };
}

llama::float4x4 llama::float4x4::operator*(float value)
{
    return float4x4(_mm_mul_ps(m_sse[0], _mm_set_ps1(value)),
                    _mm_mul_ps(m_sse[1], _mm_set_ps1(value)),
                    _mm_mul_ps(m_sse[2], _mm_set_ps1(value)),
                    _mm_mul_ps(m_sse[3], _mm_set_ps1(value)));
}

llama::float4x4 llama::float4x4::operator+=(const float4x4& matrix)
{
    return *this = float4x4(_mm_add_ps(m_sse[0], matrix.m_sse[0]),
                            _mm_add_ps(m_sse[1], matrix.m_sse[1]),
                            _mm_add_ps(m_sse[2], matrix.m_sse[2]),
                            _mm_add_ps(m_sse[3], matrix.m_sse[3]));
}

llama::float4x4 llama::float4x4::operator-=(const float4x4& matrix)
{
    return *this = float4x4(_mm_sub_ps(m_sse[0], matrix.m_sse[0]),
                            _mm_sub_ps(m_sse[1], matrix.m_sse[1]),
                            _mm_sub_ps(m_sse[2], matrix.m_sse[2]),
                            _mm_sub_ps(m_sse[3], matrix.m_sse[3]));
}

llama::float4x4 llama::float4x4::operator*=(const float4x4& matrix)
{
    return *this =
    {
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a1), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b1), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c1), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d1), matrix.m_sse[3]))
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a2), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b2), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c2), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d2), matrix.m_sse[3]))
        ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a3), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b3), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c3), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d3), matrix.m_sse[3]))
    ),
        _mm_add_ps(
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(a4), matrix.m_sse[0]),
                _mm_mul_ps(_mm_set_ps1(b4), matrix.m_sse[1])),
            _mm_add_ps(
                _mm_mul_ps(_mm_set_ps1(c4), matrix.m_sse[2]),
                _mm_mul_ps(_mm_set_ps1(d4), matrix.m_sse[3]))
        )
    };
}

llama::float4x4 llama::float4x4::operator*=(float value)
{
    return *this = float4x4(_mm_mul_ps(m_sse[0], _mm_set_ps1(value)),
                            _mm_mul_ps(m_sse[1], _mm_set_ps1(value)),
                            _mm_mul_ps(m_sse[2], _mm_set_ps1(value)),
                            _mm_mul_ps(m_sse[3], _mm_set_ps1(value)));
}

llama::float4x4 llama::scaling3D(const float3& sc)
{
    return float4x4(sc.x, 0.0f, 0.0f, 0.0f,
                    0.0f, sc.y, 0.0f, 0.0f,
                    0.0f, 0.0f, sc.z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

llama::float4x4 llama::translation3D(const float3& tr)
{
    return float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    tr.x, tr.y, tr.z, 1.0f);
}

llama::float4x4 llama::rotationX3D(float radians)
{
    return float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, ::cosf(radians), ::sinf(radians), 0.0f,
                    0.0f, -::sinf(radians), ::cosf(radians), 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

llama::float4x4 llama::rotationY3D(float radians)
{
    return float4x4(::cosf(radians), 0.0f, -::sinf(radians), 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    ::sinf(radians), 0.0f, ::cosf(radians), 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

llama::float4x4 llama::rotationZ3D(float radians)
{
    return float4x4(::cosf(radians), ::sinf(radians), 0.0f, 0.0f,
                    -::sinf(radians), ::cosf(radians), 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
}

llama::float4x4 llama::view(const float3& position, const float3& rr) // { yaw, pitch, roll }
{
    float3 forward
    {
        ::cosf(rr.x) * ::cosf(rr.y),
        ::sinf(rr.y),
        ::sinf(rr.x) * ::cosf(rr.y)
    };

    float3 right
    {
        ::cosf(rr.x + PI / 2) * ::cosf(rr.z),
        ::sinf(rr.z),
        ::sinf(rr.x + PI / 2) * ::cosf(rr.z)
    };

    float3 up = cross(forward, right);

    return float4x4(right.x, up.x, forward.x, 0.0f,
                    right.y, up.y, forward.y, 0.0f,
                    right.z, up.z, forward.z, 0.0f,
                    -dot(right, position), -dot(up, position), -dot(forward, position), 1.0f);
}

llama::float4x4 llama::projection(float aspectRatio, float fovRadians, float np, float fp)
{
    float a = 1.0f / ::tanf(0.5f * fovRadians);
    float b = fp / (np - fp);

    // For left-handed projection change signs for [2][3] and [3][2]

    return float4x4(a / aspectRatio, 0.0f, 0.0f, 0.0f,
                    0.0f, a, 0.0f, 0.0f,
                    0.0f, 0.0f, b, -1.0f,
                    0.0f, 0.0f, np * b, 0.0f);
}