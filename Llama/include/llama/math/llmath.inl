
uint8_t llama::hammingWeight(uint32_t bitfield)
{
    return static_cast<uint32_t>(_mm_popcnt_u32(bitfield));
}

uint8_t llama::hammingWeight(uint64_t bitfield)
{
    return static_cast<uint32_t>(_mm_popcnt_u64(bitfield));
}