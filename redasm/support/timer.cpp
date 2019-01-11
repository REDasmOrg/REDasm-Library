#include "timer.h"
#include "../redasm_runtime.h"

namespace REDasm {

Timer::Timer(): m_state(Timer::InactiveState) { }
Timer::~Timer() { m_state = Timer::InactiveState; }

size_t Timer::state() const { return m_state; }
bool Timer::active() const { return m_state == Timer::ActiveState; }

void Timer::stop()
{
    if(m_state == Timer::InactiveState)
        return;

    m_state = Timer::InactiveState;
    stateChanged(this);
}

void Timer::pause()
{
    if(m_state != Timer::ActiveState)
        return;

    m_state = Timer::PausedState;
    stateChanged(this);
}

void Timer::resume()
{
    if(m_state != Timer::PausedState)
        return;

    m_state = Timer::ActiveState;
    stateChanged(this);
}

void Timer::tick(TimerCallback cb, std::chrono::milliseconds interval, std::chrono::milliseconds delaystart)
{
    if(m_state != Timer::InactiveState)
        return;

    m_interval = interval;
    m_delaystart = delaystart;
    m_state = Timer::ActiveState;
    m_timercallback = cb;
    stateChanged(this);

    if(REDasm::Runtime::sync())
    {
        this->workSync();
        return;
    }

    m_future = std::async(&Timer::work, this);
}

void Timer::work()
{
    if(m_delaystart.count())
        std::this_thread::sleep_for(m_delaystart);

    while(m_state != Timer::InactiveState)
    {
        if(m_state == Timer::ActiveState)
            m_timercallback();

        std::this_thread::sleep_for(m_interval);
    }
}

void Timer::workSync()
{
    while(m_state != Timer::InactiveState)
    {
        if(m_state == Timer::ActiveState)
            m_timercallback();
    }
}

} // namespace REDasm
