#pragma once

#include <condition_variable>
#include <functional>
#include <atomic>
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
        size_t id() const;
        bool active() const;
        void start();
        void stop();
        void pause();
        void resume();
        void setOneShot(bool b);
        void work(const JobCallback &cb, bool deferred = false);

    private:
        void sleep();
        void doWork();
        void doWorkSync();

    private:
        bool m_oneshot;
        std::atomic<size_t> m_state;
        std::chrono::milliseconds m_interval;
        JobCallback m_jobcallback;
        std::condition_variable m_cv;
        std::thread m_thread;
        std::mutex m_mutex;
        size_t m_id;

    private:
        static size_t m_jobid;
};

} // namespace REDasm
