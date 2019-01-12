#include "concurrenttimer.h"
#include "../redasm_runtime.h"
#include <algorithm>
#include <thread>

namespace REDasm {

ConcurrentTimer::ConcurrentTimer()
{
    size_t concurrency = std::thread::hardware_concurrency();

    if(!concurrency || REDasm::Runtime::sync())
        concurrency = 1;

    for(size_t i = 0; i < concurrency; i++)
        m_timers.push_back(std::make_unique<Timer>());

    m_timers.back()->stateChanged += [=](Timer* timer) {
        stateChanged(timer);
    };
}

u64 ConcurrentTimer::interval() const { return m_interval.count(); }
size_t ConcurrentTimer::concurrency() const { return m_timers.size(); }
size_t ConcurrentTimer::state() const { return m_timers.back()->state(); }
bool ConcurrentTimer::active() const { return m_timers.back()->active(); }

bool ConcurrentTimer::setSelfBalance(bool b)
{
    for(auto& timer : m_timers)
        timer->setSelfBalance(b);
}

void ConcurrentTimer::stop()
{
    for(auto& timer : m_timers)
        timer->stop();
}

void ConcurrentTimer::pause()
{
    for(auto& timer : m_timers)
        timer->pause();
}

void ConcurrentTimer::resume()
{
    for(auto& timer : m_timers)
        timer->resume();
}

void ConcurrentTimer::tick(TimerCallback cb, std::chrono::milliseconds interval)
{
    for(auto& timer : m_timers)
        timer->tick(cb, interval);
}

} // namespace REDasm
