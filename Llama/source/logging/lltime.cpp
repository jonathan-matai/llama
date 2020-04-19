#include "llcore.h"

#ifdef LLAMA_OS_WINDOWS

// Makes the functionallity of localtime_s the same as localtime_r on unix
tm* localtime_r(const time_t* time, struct tm* buf)
{
    if (localtime_s(buf, time) == 0)
        return buf;
    else
        return nullptr;
}

#endif

llama::Timestamp::Timestamp() :
    m_time{}
{

    if (timespec_get(&m_time, TIME_UTC) == 0)
        printf("Error at %s:%d (%s): timepec_get() failed!\n", std::filesystem::path(__FILE__).filename().string().c_str(), __LINE__, __FUNCTION__);
}

std::string llama::Timestamp::getString(const char* format, TimeAccuracy accuracy) const
{
    tm details;
    if (localtime_r(&m_time.tv_sec, &details) == nullptr)
        printf("Error at %s:%d (%s): localtime_r() failed!\n", std::filesystem::path(__FILE__).filename().string().c_str(), __LINE__, __FUNCTION__);

    char buffer[256];

    size_t length = strftime(buffer, 256, format, &details);

    if (length == 0)
        printf("Error at %s:%d (%s): strftime() failed!\n", std::filesystem::path(__FILE__).filename().string().c_str(), __LINE__, __FUNCTION__);

    switch (accuracy)
    {
    case TimeAccuracy::SECONDS:
        break;
    case TimeAccuracy::MILLISECONDS:
        snprintf(&buffer[length], 256 - length, ".%03ld", m_time.tv_nsec / 1'000'000);
        break;
    case TimeAccuracy::MICROSECONDS:
        snprintf(&buffer[length], 256 - length, ".%06ld", m_time.tv_nsec / 1'000);
        break;
    case TimeAccuracy::NANOSECONDS:
        snprintf(&buffer[length], 256 - length, ".%09ld", m_time.tv_nsec);
        break;
    }

    return std::string(buffer);
}

std::string llama::duration(const Timestamp& start, const Timestamp& end)
{
    uint64_t dur = (end.m_time.tv_sec - start.m_time.tv_sec) * 1'000'000'000 + (end.m_time.tv_nsec - start.m_time.tv_nsec);

    // "xxx.xxx μs", "xx h xx min" (12 chars)
    std::string formatted(12, '\0');

    if (dur < 1'000) // < 1μs
        snprintf(&formatted[0], 12, "%u ns", static_cast<uint32_t>(dur));
    else if (dur < 1'000'000) // < 1ms
        snprintf(&formatted[0], 12, u8"%.3f μs", static_cast<float>(dur) / 1'000.0f);
    else if (dur < 1'000'000'000) // < 1s
        snprintf(&formatted[0], 12, "%.3f ms", static_cast<float>(dur) / 1'000'000.0f);
    else if (dur < 60'000'000'000) // < 1min
        snprintf(&formatted[0], 12, "%.3f s", static_cast<float>(dur) / 1'000'000'000.0f);
    else if ((dur /= 1'000'000'000) < 3600) // < 1h
        snprintf(&formatted[0], 12, "%lld:%02lld min", dur, dur /= 60);
    else if (dur < 60 * 24) // < 24h
        snprintf(&formatted[0], 12, "%lld:%02lld h", dur, dur /= 60);
    else if ((dur /= 24) < 31) // < 1m
        snprintf(&formatted[0], 12, "%lld d", dur);
    else if ((dur /= 31) < 12) // < 1y
        snprintf(&formatted[0], 12, "%lld m", dur);
    else // > 1y
        snprintf(&formatted[0], 12, "%lld y", dur / 12);

    return std::move(formatted);
}

uint64_t llama::duration(const Timestamp& start, const Timestamp& end, TimeAccuracy accuracy)
{
    uint64_t time = (end.m_time.tv_sec - start.m_time.tv_sec) * 1'000'000'000 + (end.m_time.tv_nsec - start.m_time.tv_nsec);

    switch (accuracy)
    {
    case TimeAccuracy::SECONDS:
        return time / 1'000'000'000;
    case TimeAccuracy::MILLISECONDS:
        return time / 1'000'000;
    case TimeAccuracy::MICROSECONDS:
        return time / 1'000;
    case TimeAccuracy::NANOSECONDS:
    default:
        return time;
    }
}