#pragma once

#include <redasm/support/jobmanager.h>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include "ringbuffer.h"

namespace REDasm {

class JobManagerImpl
{
    public:
        static constexpr size_t MAX_JOBS = 256;
        using job_lock = std::unique_lock<std::mutex>;

    public:
        JobManagerImpl() = delete;
        static void waitTerminate();
        static bool terminated();
        static void poll();

    public:
        static RingBuffer<JobManager::JobCallback, MAX_JOBS> m_jobpool;
        static std::condition_variable m_wakecondition;
        static std::mutex m_wakemutex;
        static std::atomic_size_t m_finishedlabel, m_numthreads;
        static std::atomic_bool m_initialized;
        static size_t m_currentlabel;
};

} // namespace REDasm

