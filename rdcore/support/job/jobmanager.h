#pragma once

// Based on: https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/

#include <condition_variable>
#include <functional>
#include <atomic>
#include <mutex>
#include "ringbuffer.h"

struct JobDispatchArgs { size_t jobindex, groupindex; };

class JobManager
{
    public:
        static constexpr size_t MAX_JOBS = 256;
        using job_lock = std::unique_lock<std::mutex>;

    public:
        typedef std::function<void()> JobCallback;
        typedef std::function<void(const JobDispatchArgs&)> JobDispatchCallback;

    public:
        JobManager() = delete;
        static size_t concurrency();
        static bool initialized();
        static bool last();
        static bool busy();
        static void wait();
        static void dispatch(const JobDispatchCallback& cb);
        static void dispatch(size_t jobcount, const JobDispatchCallback& cb);
        static void dispatch(size_t jobcount, size_t groupsize, const JobDispatchCallback& cb);
        static void execute(const JobCallback& cb);
        static void deinitialize();
        static void initialize();

    private:
        static void waitTerminate();
        static bool terminated();
        static void poll();

    public:
        template<typename Class, typename Member> static void execute(Class* c, const Member& m) {
            JobManager::execute(std::bind(m, c));
        }

        template<typename Class, typename Member> static void dispatch(Class* c, const Member& m) {
            JobManager::dispatch(std::bind(m, c, std::placeholders::_1));
        }

        template<typename Class, typename Member> static void dispatch(size_t jobcount, Class* c, const Member& m) {
            JobManager::dispatch(jobcount, std::bind(m, c, std::placeholders::_1));
        }

        template<typename Class, typename Member> static void dispatch(size_t jobcount, size_t groupsize, Class* c, const Member& m) {
            JobManager::dispatch(jobcount, groupsize, std::bind(m, c, std::placeholders::_1));
        }

    private:
        static RingBuffer<JobManager::JobCallback, MAX_JOBS> m_jobpool;
        static std::condition_variable m_wakecondition;
        static std::mutex m_wakemutex;
        static std::atomic_size_t m_finishedlabel, m_numthreads;
        static std::atomic_bool m_initialized;
        static size_t m_currentlabel;
};
