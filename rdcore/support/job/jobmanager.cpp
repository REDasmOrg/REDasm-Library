#include "jobmanager.h"
#include <algorithm>
#include <thread>
#include <cmath>

constexpr size_t JobManager::MAX_JOBS;
RingBuffer<JobManager::JobCallback, JobManager::MAX_JOBS> JobManager::m_jobpool;
size_t JobManager::m_currentlabel{0};
std::condition_variable JobManager::m_wakecondition;
std::mutex JobManager::m_wakemutex;
std::atomic_size_t JobManager::m_finishedlabel{0}, JobManager::m_numthreads{0};
std::atomic_bool JobManager::m_initialized;

size_t JobManager::concurrency() { return m_numthreads.load(); }
bool JobManager::initialized() { return m_initialized; }
bool JobManager::last() { return (m_currentlabel - m_finishedlabel.load()) == 1; }
bool JobManager::busy() { return m_finishedlabel.load() < m_currentlabel; }
void JobManager::wait() { while(JobManager::busy()) JobManager::poll(); }
void JobManager::dispatch(const JobManager::JobDispatchCallback& cb) { JobManager::dispatch(m_numthreads.load(), cb); }
void JobManager::dispatch(size_t jobcount, const JobManager::JobDispatchCallback& cb) { JobManager::dispatch(jobcount, 1, cb); }

/*
 * Divide a job onto multiple jobs and execute in parallel.
 * - jobcount  : How many jobs to generate for this task.
 * - groupsize : How many jobs to execute per thread. Jobs inside a group execute serially.
 */
void JobManager::dispatch(size_t jobcount, size_t groupsize, const JobManager::JobDispatchCallback& cb)
{
    if(!jobcount || !groupsize) return;

    size_t groupcount = static_cast<size_t>(std::ceil((jobcount + groupsize - 1) / groupsize));
    m_currentlabel += groupcount;

    for(size_t groupindex = 0; groupindex < groupcount; groupindex++)
    {
        auto jobgroup = [jobcount, groupsize, cb, groupindex]() {
            size_t groupjoboffset = groupindex * groupsize;
            size_t groupjobend = std::min(groupjoboffset + groupsize, jobcount);

            JobDispatchArgs args;
            args.groupindex = groupindex;

            for(size_t i = groupjoboffset; i < groupjobend; ++i) {
                args.jobindex = i;
                cb(args);
            }
        };

        while(!m_jobpool.push_back(jobgroup))
            JobManager::poll();

        m_wakecondition.notify_one(); // wake one thread
    }
}

void JobManager::execute(const JobManager::JobCallback& cb)
{
    m_currentlabel += 1;

    // Try to push a new job until it is pushed successfully:
    while(!m_jobpool.push_back(cb))
        JobManager::poll();

    m_wakecondition.notify_one();
}

void JobManager::deinitialize()
{
    m_initialized.store(false);  // Notify threads for deinitialization
    JobManager::wait();          // Wait for running jobs
    JobManager::waitTerminate(); // Wait for thread termination
}

void JobManager::initialize()
{
    m_numthreads.store(std::max(1u, std::thread::hardware_concurrency()));
    m_jobpool.reset();
    m_currentlabel = 0;
    m_finishedlabel.store(0);
    m_initialized.store(true);

    for(size_t tid = 0; tid < m_numthreads; tid++)
    {
        std::thread worker([] {
            JobCallback job;

            while(m_initialized) {
                if(m_jobpool.pop_front(job)) {
                    job();
                    m_finishedlabel.fetch_add(1);
                    continue;
                }

                job_lock lock(m_wakemutex);
                m_wakecondition.wait(lock);
            }

            m_numthreads.fetch_sub(1);
        });

        worker.detach();
    }
}

void JobManager::waitTerminate() { while(!JobManager::terminated()) JobManager::poll(); }
bool JobManager::terminated() { return m_numthreads.load() == 0;  }

void JobManager::poll()
{
    m_wakecondition.notify_one();
    std::this_thread::yield();
}
