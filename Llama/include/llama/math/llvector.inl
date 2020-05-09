/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llvector.inl
 The Implementation of Vector Classes of the Llama Math Library
*/

/*
    SSE Intrinsics Cheat Sheet

    Explanation of third parameter of the function _mm_dp_ps:
    0b[0][1][2][3][4][5][6][7]

    [0]: calculate with w-component
    [1]: calculate with z-component
    [2]: calculate with y-component
    [3]: calculate with x-component

    [4]: write output to w-component
    [5]: write output to z-component
    [6]: write output to y-component
    [7]: write output to x-component

    Explanation of second parameter of the function _mm_permute_ps:

    0b[01][23][45][67]

    [01] The value stored in w (00 = x, 01 = y, 10 = z, 11 = w)
    [23] The value stored in z (00 = x, 01 = y, 10 = z, 11 = w)
    [45] The value stored in y (00 = x, 01 = y, 10 = z, 11 = w)
    [67] The value stored in w (00 = x, 01 = y, 10 = z, 11 = w)
    */

inline llama::float2::float2()                                      { m_sse = _mm_set_ps1(0.0f); }
inline llama::float2::float2(const float2& other)                   { m_sse = other.m_sse; }
inline llama::float2::float2(float value)                           { m_sse = _mm_set_ps(0.0f, 0.0f, value, value); }
inline llama::float2::float2(float x, float y)                      { m_sse = _mm_set_ps(0.0f, 0.0f, y, x); }

llama::float2 llama::float2::operator+(const float2& vec)           { return _mm_add_ps(m_sse, vec.m_sse); }
llama::float2 llama::float2::operator-(const float2& vec)           { return _mm_sub_ps(m_sse, vec.m_sse); }
llama::float2 llama::float2::operator*(const float2& vec)           { return _mm_mul_ps(m_sse, vec.m_sse); }
llama::float2 llama::float2::operator*(float value)                 { return _mm_mul_ps(m_sse, _mm_set_ps1(value)); }
llama::float2 llama::float2::operator/(const float2& vec)           { return _mm_div_ps(m_sse, vec.m_sse); }
llama::float2 llama::float2::operator/(float value)                 { return _mm_mul_ps(m_sse, _mm_set_ps(1.0f, 1.0f, value, value)); }

llama::float2& llama::float2::operator+=(const float2& vec)         { return *this = _mm_add_ps(m_sse, vec.m_sse); }
llama::float2& llama::float2::operator-=(const float2& vec)         { return *this = _mm_sub_ps(m_sse, vec.m_sse); }
llama::float2& llama::float2::operator*=(const float2& vec)         { return *this = _mm_mul_ps(m_sse, vec.m_sse); }
llama::float2& llama::float2::operator*=(float value)               { return *this = _mm_mul_ps(m_sse, _mm_set_ps1(value)); }
llama::float2& llama::float2::operator/=(const float2& vec)         { return *this = _mm_div_ps(m_sse, vec.m_sse); }
llama::float2& llama::float2::operator/=(float value)               { return *this = _mm_mul_ps(m_sse, _mm_set_ps(1.0f, 1.0f, value, value)); }

llama::float2 llama::operator*(float value, const float2& vec)      { return _mm_mul_ps(vec.m_sse, _mm_set_ps1(value)); }
float llama::length(const float2& vec)                              { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(vec.m_sse, vec.m_sse, 0b00110001))); }
llama::float2 llama::normalize(const float2& vec)                   { return _mm_mul_ps(vec.m_sse, _mm_rsqrt_ps(_mm_dp_ps(vec.m_sse, vec.m_sse, 0b00110011))); }
float llama::dot(const float2& a, const float2& b)                  { return _mm_cvtss_f32(_mm_dp_ps(a.m_sse, b.m_sse, 0b00110001)); }
float llama::cross(const float2& a, const float2& b)                { return a.x * b.y - a.y - b.x; }

llama::float3::float3()                                             { m_sse = _mm_set_ps1(0.0f); }
llama::float3::float3(const float3& other)                          { m_sse = other.m_sse; }
llama::float3::float3(float value)                                  { m_sse = _mm_set_ps(0.0f, value, value, value); }
llama::float3::float3(float x, float y, float z)                    { m_sse = _mm_set_ps(0.0f, z, y, x); }

llama::float3 llama::float3::operator+(const float3& vec)           { return _mm_add_ps(m_sse, vec.m_sse); }
llama::float3 llama::float3::operator-(const float3& vec)           { return _mm_sub_ps(m_sse, vec.m_sse); }
llama::float3 llama::float3::operator*(const float3& vec)           { return _mm_mul_ps(m_sse, vec.m_sse); }
llama::float3 llama::float3::operator*(float value)                 { return _mm_mul_ps(m_sse, _mm_set_ps1(value)); }
llama::float3 llama::float3::operator/(const float3& vec)           { return _mm_div_ps(m_sse, vec.m_sse); }
llama::float3 llama::float3::operator/(float value)                 { return _mm_mul_ps(m_sse, _mm_set_ps(1.0f, value, value, value)); }

llama::float3& llama::float3::operator+=(const float3& vec)         { return *this = _mm_add_ps(m_sse, vec.m_sse); }
llama::float3& llama::float3::operator-=(const float3& vec)         { return *this = _mm_sub_ps(m_sse, vec.m_sse); }
llama::float3& llama::float3::operator*=(const float3& vec)         { return *this = _mm_mul_ps(m_sse, vec.m_sse); }
llama::float3& llama::float3::operator*=(float value)               { return *this = _mm_mul_ps(m_sse, _mm_set_ps1(value)); }
llama::float3& llama::float3::operator/=(const float3& vec)         { return *this = _mm_div_ps(m_sse, vec.m_sse); }
llama::float3& llama::float3::operator/=(float value)               { return *this = _mm_mul_ps(m_sse, _mm_set_ps(1.0f, value, value, value)); }

llama::float3 llama::operator*(float value, const float3& vec)      { return _mm_mul_ps(vec.m_sse, _mm_set_ps1(value)); }
float llama::length(const float3& vec)                              { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(vec.m_sse, vec.m_sse, 0b01110001))); }
llama::float3 llama::normalize(const float3& vec)                   { return _mm_mul_ps(vec.m_sse, _mm_rsqrt_ps(_mm_dp_ps(vec.m_sse, vec.m_sse, 0b01110011))); }
float llama::dot(const float3& a, const float3& b)                  { return _mm_cvtss_f32(_mm_dp_ps(a.m_sse, b.m_sse, 0b01110001)); }

llama::float3 llama::cross(const float3& a, const float3& b)
{
    return _mm_sub_ps(
        _mm_mul_ps(
            _mm_permute_ps(a.m_sse, 0b11001001),
            _mm_permute_ps(b.m_sse, 0b11010010)
        ),
        _mm_mul_ps(
            _mm_permute_ps(a.m_sse, 0b11010010),
            _mm_permute_ps(b.m_sse, 0b11001001)
        )
    );
}


llama::float4::float4()                                             { m_sse = _mm_set_ps1(0.0f); }
llama::float4::float4(const float4& other)                          { m_sse = other.m_sse; }
llama::float4::float4(float value)                                  { m_sse = _mm_set_ps1(value); }
llama::float4::float4(float x, float y, float z, float w)           { m_sse = _mm_set_ps(w, z, y, x); }

llama::float4 llama::float4::operator+(const float4& vec)           { return _mm_add_ps(m_sse, vec.m_sse); }
llama::float4 llama::float4::operator-(const float4& vec)           { return _mm_sub_ps(m_sse, vec.m_sse); }
llama::float4 llama::float4::operator*(const float4& vec)           { return _mm_mul_ps(m_sse, vec.m_sse); }
llama::float4 llama::float4::operator*(float value)                 { return _mm_mul_ps(m_sse, _mm_set_ps1(value)); }
llama::float4 llama::float4::operator/(const float4& vec)           { return _mm_div_ps(m_sse, vec.m_sse); }
llama::float4 llama::float4::operator/(float value)                 { return _mm_mul_ps(m_sse, _mm_set_ps1(value)); }

llama::float4& llama::float4::operator+=(const float4& vec)         { return *this = _mm_add_ps(m_sse, vec.m_sse); }
llama::float4& llama::float4::operator-=(const float4& vec)         { return *this = _mm_sub_ps(m_sse, vec.m_sse); }
llama::float4& llama::float4::operator*=(const float4& vec)         { return *this = _mm_mul_ps(m_sse, vec.m_sse); }
llama::float4& llama::float4::operator*=(float value)               { return *this = _mm_mul_ps(m_sse, _mm_set_ps1(value)); }
llama::float4& llama::float4::operator/=(const float4& vec)         { return *this = _mm_div_ps(m_sse, vec.m_sse); }
llama::float4& llama::float4::operator/=(float value)               { return *this = _mm_mul_ps(m_sse, _mm_set_ps1(value)); }

llama::float4 llama::operator*(float value, const float4& vec)      { return _mm_mul_ps(vec.m_sse, _mm_set_ps1(value)); }
float llama::length(const float4& vec)                              { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(vec.m_sse, vec.m_sse, 0b11110001))); }
llama::float4 llama::normalize(const float4& vec)                   { return _mm_mul_ps(vec.m_sse, _mm_rsqrt_ps(_mm_dp_ps(vec.m_sse, vec.m_sse, 0b11110011))); }
float llama::dot(const float4& a, const float4& b)                  { return _mm_cvtss_f32(_mm_dp_ps(a.m_sse, b.m_sse, 0b11110001)); }