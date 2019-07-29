#pragma once

#include <condition_variable>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <redasm/disassembler/concurrent/job.h>
#include <redasm/pimpl.h>

namespace REDasm {

class JobImpl
{
    PIMPL_DECLARE_Q(Job)
    PIMPL_DECLARE_PUBLIC(Job)

    public:
        JobImpl(Job* q);
        ~JobImpl();
        JobState state() const;
        size_t id() const;
        bool active() const;
        void start();
        void stop();
        void pause();
        void resume();
        void setOneShot(bool b);
        void work(const Job::JobCallback &cb, bool deferred = false);

    private:
        void sleep();
        void doWork();
        void doWorkSync();

    private:
        bool m_oneshot;
        std::atomic<JobState> m_state;
        std::chrono::milliseconds m_interval;
        Job::JobCallback m_jobcallback;
        std::condition_variable m_cv;
        std::thread m_thread;
        std::mutex m_mutex;
        size_t m_id;

    private:
        static size_t m_jobid;
};

} // namespace REDasm
