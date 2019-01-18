#include "jobspool.h"
#include "../../redasm_runtime.h"
#include <iostream>
#include <algorithm>
#include <thread>

namespace REDasm {

JobsPool::JobsPool(): m_running(true)
{
    m_concurrency = std::thread::hardware_concurrency();

    if(!m_concurrency || REDasm::Runtime::sync())
        m_concurrency = 1;

    for(size_t i = 0; i < m_concurrency; i++)
    {
        auto job = std::make_unique<Job>();

        job->stateChanged += [&](Job* job) {
            stateChanged(job);
        };

        m_jobs.push_back(std::move(job));
    }
}

JobsPool::~JobsPool() { m_running = false; }
size_t JobsPool::concurrency() const { return m_jobs.size(); }

size_t JobsPool::activeCount() const
{
    size_t i = 0;

    for(auto& job : m_jobs)
    {
        if(job->active())
            i++;
    }

    return i;
}

size_t JobsPool::state() const
{
    size_t s = Job::InactiveState;

    for(auto& job : m_jobs)
    {
        if(job->state() == Job::InactiveState)
            continue;

        s = job->state();

        if(job->state() != Job::ActiveState)
            break;
    }

    return s;
}

bool JobsPool::active() const
{
    if(!m_running)
        return false;

    for(auto& job : m_jobs)
    {
        if(job->active())
            return true;
    }

    return false;
}

void JobsPool::stop()
{
    for(auto& job : m_jobs)
        job->stop();

    stateChanged(m_jobs.back().get());
}

void JobsPool::pause()
{
    for(auto& job : m_jobs)
        job->pause();

    stateChanged(m_jobs.back().get());
}

void JobsPool::resume()
{
    for(auto& job : m_jobs)
        job->resume();

    stateChanged(m_jobs.back().get());
}

void JobsPool::work(JobCallback cb)
{
    for(auto& job : m_jobs)
        job->work(cb);

    stateChanged(m_jobs.back().get());
}

} // namespace REDasm
