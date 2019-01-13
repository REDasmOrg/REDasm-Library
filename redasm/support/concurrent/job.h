#ifndef JOB_H
#define JOB_H

#include <condition_variable>
#include <functional>
#include <thread>
#include <mutex>
#include "../event.h"

namespace REDasm {

class Job;
typedef std::function<void(Job*)> JobCallback;

class Job
{
    public:
        Event<Job*> stateChanged;

    public:
        enum : size_t { InactiveState = 0, SleepState, ActiveState, PausedState, };

    public:
        Job();
        ~Job();
        size_t state() const;
        bool active() const;
        void setSelfBalance(bool b);
        void start();
        void stop();
        void pause();
        void resume();
        void work(JobCallback cb, bool deferred = false);

    private:
        void doWork();
        void doWorkSync();

    private:
        size_t m_state;
        std::chrono::milliseconds m_interval;
        JobCallback m_jobcallback;
        std::condition_variable m_cv;
        std::thread m_thread;
        std::mutex m_mutex;
};

} // namespace REDasm

#endif // JOB_H
