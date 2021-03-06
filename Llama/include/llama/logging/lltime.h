﻿/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > lltime.h
 Time Measurement Tools
*/

#pragma once

namespace llama
{
    enum class TimeAccuracy
    {
        SECONDS,
        MILLISECONDS,
        MICROSECONDS,
        NANOSECONDS
    };

    class Timestamp;

    // Returns the time passed between two timestamps as a string
    // @start:      Start of measurement
    // @end:        End of measurement
    // @return:     Time difference as formatted string (000.000 μs, 000.000 ms, 00.000 s, 00 min 00 s, 00 h 00 min)
    LLAMA_API std::string duration(const Timestamp& start, const Timestamp& end);

    // Returns the time passed between two timestamps as an 64bit integer
    // @start:      Start of measurement
    // @end:        End of measurement
    // @accuracy:   Format in which time is returned
    LLAMA_API uint64_t duration(const Timestamp& start, const Timestamp& end, TimeAccuracy accuracy);

    class Timestamp
    {
    public:

        // Intializes Timestamp with current Time
        LLAMA_API Timestamp();

        // Returns a formatted time string of the timestamp
        // @format:     UTF-8 format string (see format parameter of strftime: https://en.cppreference.com/w/cpp/chrono/c/strftime)
        // @accuracy:   Describes how accurate the timestamp should be printed
        // @return:     Returns an UTF-8-encoded Time string
        LLAMA_API std::string getString(const char* format, TimeAccuracy accuracy) const;

        LLAMA_API void modify(uint64_t amount, TimeAccuracy unit);

        inline bool operator==(const Timestamp& other) { return other.m_time.tv_sec == m_time.tv_sec && other.m_time.tv_nsec == m_time.tv_nsec; }

        inline bool operator>(const Timestamp& other)
        {
            if (m_time.tv_sec > other.m_time.tv_sec)
                return true;
            if (m_time.tv_sec < other.m_time.tv_sec)
                return false;

            return m_time.tv_nsec > other.m_time.tv_nsec;
        }

        inline bool operator<(const Timestamp& other)
        {
            if (m_time.tv_sec > other.m_time.tv_sec)
                return false;
            if (m_time.tv_sec < other.m_time.tv_sec)
                return true;

            return m_time.tv_nsec < other.m_time.tv_nsec;
        }

        inline bool operator>=(const Timestamp& other)
        {
            if (m_time.tv_sec > other.m_time.tv_sec)
                return true;
            if (m_time.tv_sec < other.m_time.tv_sec)
                return false;

            return m_time.tv_nsec >= other.m_time.tv_nsec;
        }

        inline bool operator<=(const Timestamp& other)
        {
            if (m_time.tv_sec > other.m_time.tv_sec)
                return false;
            if (m_time.tv_sec < other.m_time.tv_sec)
                return true;

            return m_time.tv_nsec <= other.m_time.tv_nsec;
        }

        // Friend declarations
        LLAMA_API friend std::string duration(const Timestamp& start, const Timestamp& end);
        LLAMA_API friend uint64_t duration(const Timestamp& start, const Timestamp& end, TimeAccuracy accuracy);

    private:

        // The time in nanoseconds since Epoch
        timespec m_time;
    };
}