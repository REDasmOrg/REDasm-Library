#ifndef CONCURRENTTIMER_H
#define CONCURRENTTIMER_H

#include <memory>
#include <list>
#include "../redasm_api.h"
#include "timer.h"
#include "event.h"

namespace REDasm {

class ConcurrentTimer
{
    public:
        Event<Timer*> stateChanged;

    public:
        ConcurrentTimer();
        u64 interval() const;
        size_t concurrency() const;
        size_t state() const;
        bool active() const;
        void stop();
        void pause();
        void resume();
        void tick(TimerCallback cb, std::chrono::milliseconds interval = std::chrono::milliseconds(TIMER_INTERVAL));

    private:
        std::list< std::unique_ptr<Timer> > m_timers;
        std::chrono::milliseconds m_interval;
};

} // namespace REDasm

#endif // CONCURRENTTIMER_H
