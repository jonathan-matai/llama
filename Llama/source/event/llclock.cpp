#include "llpch.h"
#include "event/llclock.h"

#include "math/llmath.h"

namespace llama
{
    class Clock_I : public Clock_T
    {
    public:
        
        Clock_I(EventBus bus, std::initializer_list<float> tickrates);
        
        void run() override;

    private:

        struct Tickrate
        {
            // When the last tick happened
            Timestamp lastTick;

            // When the next tick should happen
            Timestamp nexTickDueTime;

            // The ideal Interval between two ticks
            uint64_t maxTickDurationNs;

            // Get incremented every tick
            uint64_t tickCount;

            // The value of 'tickCount' when stats were retrieved the last time
            // Used for statistics
            uint64_t lastTickCount;

            Tickrate(float tps) :
                maxTickDurationNs(tps == 0.0f ? 0 : static_cast<uint64_t>(1'000'000'000 / tps)),
                tickCount(0),
                lastTickCount(0)
            { }
        };

        EventDispatchState onClose(CloseApplicationEvent* e);

        EventBus m_eventBus;
        EventDispatchFunction m_onClose;
        bool m_running;

        std::vector<Tickrate> m_tickrates;

        // In Seconds
        static constexpr float s_tickrateReportInterval = 2.0f;
        Timestamp m_lastTickrateReport;

        // If a tick should have happened more than this time ago, ticks will get skipped
        static constexpr uint32_t maxTimeBehindMs = 500;
    };
    
}

llama::Clock_I::Clock_I(EventBus bus, std::initializer_list<float> tickrates) :
    m_eventBus(bus),
    m_onClose(bus, this, &Clock_I::onClose),
    m_running(false)
{
    m_tickrates.reserve(tickrates.size());

    for (auto a : tickrates)
        m_tickrates.push_back(Tickrate(a));
}

void llama::Clock_I::run()
{
    if (m_running)
    {
        logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "run() was called on llama::Clock %p, that was already running!", this);
        return;
    }

    m_running = true;

    // Reset last tick
    {
        Timestamp now;
        for (auto& a : m_tickrates)
            a.lastTick = a.nexTickDueTime = now;

        m_lastTickrateReport = now;
    }
    
    while (m_running)
    {
        Timestamp now;

        for (uint32_t i = 0; i < m_tickrates.size(); ++i)
        {
            if(m_tickrates[i].maxTickDurationNs == 0 || now > m_tickrates[i].nexTickDueTime)
            {
                // Send the tick event
                auto duration = llama::duration(m_tickrates[i].lastTick, now, TimeAccuracy::NANOSECONDS);
                m_eventBus->postEvent(TickEvent(i, duration / 1'000'000.0f, m_tickrates[i].tickCount));

                // Increase Tick Counter
                ++m_tickrates[i].tickCount;

                // Set Time of Last Tick
                m_tickrates[i].lastTick = now;

                if (m_tickrates[i].maxTickDurationNs > 0)
                {
                    auto behind = llama::duration(m_tickrates[i].nexTickDueTime, now, TimeAccuracy::MILLISECONDS);

                    if (behind > maxTimeBehindMs)
                    {
                        m_tickrates[i].nexTickDueTime = now;
                        logfile()->print(Colors::YELLOW, LLAMA_DEBUG_INFO, "Static Tickrate #%d cannot be maintained! Skipping %d ms", i, static_cast<uint32_t>(behind));
                    }

                    // Increase due time
                    m_tickrates[i].nexTickDueTime.modify(m_tickrates[i].maxTickDurationNs, TimeAccuracy::NANOSECONDS);
                }
            }
        }

        float timeSinceLastReport = duration(m_lastTickrateReport, now, TimeAccuracy::MILLISECONDS) / 1000.0f;

        if (timeSinceLastReport > s_tickrateReportInterval)
        {
            std::string rates;

            for (uint32_t i = 0; i < m_tickrates.size(); ++i)
            {
                uint32_t tps = static_cast<uint32_t>(llama::round((m_tickrates[i].tickCount - m_tickrates[i].lastTickCount) / timeSinceLastReport));

                //uint32_t tps = static_cast<uint32_t>(m_tickrates[i].tickCount - m_tickrates[i].lastTickCount);

                rates += (std::to_string(i) + " : " + std::to_string(tps) + ", ");
                m_tickrates[i].lastTickCount = m_tickrates[i].tickCount;
            }

            rates.pop_back();
            rates.pop_back();

            m_lastTickrateReport = now;
            logfile()->print(Colors::GREY, "Tick Rates: { %s }", rates.c_str());
        }
    }
}

llama::EventDispatchState llama::Clock_I::onClose(CloseApplicationEvent* e)
{
    m_running = false;
    return EventDispatchState::PROCESSED;
}

llama::Clock llama::createClock(EventBus bus, std::initializer_list<float> tickrates)
{
    return std::make_shared<Clock_I>(bus, tickrates);
}