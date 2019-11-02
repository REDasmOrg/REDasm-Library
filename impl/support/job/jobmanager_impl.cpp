#include "jobmanager_impl.h"
#include <thread>

namespace REDasm {

constexpr size_t JobManagerImpl::MAX_JOBS;
RingBuffer<JobManager::JobCallback, JobManagerImpl::MAX_JOBS> JobManagerImpl::m_jobpool;
size_t JobManagerImpl::m_currentlabel = 0;
std::condition_variable JobManagerImpl::m_wakecondition;
std::mutex JobManagerImpl::m_wakemutex;
std::atomic_size_t JobManagerImpl::m_finishedlabel, JobManagerImpl::m_numthreads;
std::atomic_bool JobManagerImpl::m_initialized;

void JobManagerImpl::waitTerminate()
{
    while(!JobManagerImpl::terminated())
        JobManagerImpl::poll();
}

bool JobManagerImpl::terminated() { return m_numthreads.load() == 0; }

void JobManagerImpl::poll()
{
    m_wakecondition.notify_one();
    std::this_thread::yield();
}

} // namespace REDasm
