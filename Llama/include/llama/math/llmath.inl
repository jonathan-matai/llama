
uint8_t llama::hammingWeight(uint32_t bitfield)
{
    return static_cast<uint32_t>(_mm_popcnt_u32(bitfield));
}

uint8_t llama::hammingWeight(uint64_t bitfield)
{
    return static_cast<uint32_t>(_mm_popcnt_u64(bitfield));
}

template<typename compare_type>
compare_type llama::min(compare_type a, compare_type b)
{
    return a < b ? a : b;
}

template<typename compare_type>
compare_type llama::max(compare_type a, compare_type b)
{
    return a > b ? a : b;
}

template<typename compare_type>
compare_type llama::clamp(compare_type min, compare_type value, compare_type max)
{
    return value < min ? min : (value > max ? max : value);
}

float llama::round(float number)
{
    return std::roundf(number);
}

double llama::round(double number)
{
    return std::round(number);
}