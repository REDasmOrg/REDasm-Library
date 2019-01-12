#ifndef TIMER_H
#define TIMER_H

#define TIMER_INTERVAL 1 // 1ms

#include <condition_variable>
#include <functional>
#include <thread>
#include "event.h"

namespace REDasm {

typedef std::function<void()> TimerCallback;

class Timer
{
    public:
        Event<Timer*> stateChanged;

    public:
        enum : size_t { InactiveState = 0, ActiveState, PausedState };

    public:
        Timer();
        ~Timer();
        size_t state() const;
        bool active() const;
        void setSelfBalance(bool b);
        void stop();
        void pause();
        void resume();
        void tick(TimerCallback cb, std::chrono::milliseconds interval = std::chrono::milliseconds(TIMER_INTERVAL), std::chrono::milliseconds delaystart = std::chrono::milliseconds(0));

    private:
        void work();
        void workSync();

    private:
        size_t m_state;
        TimerCallback m_timercallback;
        std::chrono::milliseconds m_interval, m_delaystart;
        std::thread m_thread;
        bool m_selfbalance;
};

} // namespace REDasm

#endif // TIMER_H
