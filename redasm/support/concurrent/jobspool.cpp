#include "jobspool.h"
#include "../../redasm_runtime.h"
#include <iostream>
#include <algorithm>
#include <thread>

namespace REDasm {

JobsPool::JobsPool()
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

        m_pool.push_back(std::move(job));
    }
}

size_t JobsPool::concurrency() const { return m_pool.size(); }

size_t JobsPool::activeCount() const
{
    size_t i = 0;

    for(auto& job : m_pool)
    {
        if(job->active())
            i++;
    }

    return i;
}

size_t JobsPool::state() const
{
    size_t s = Job::InactiveState;

    for(auto& job : m_pool)
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
    for(auto& job : m_pool)
    {
        if(job->active())
            return true;
    }

    return false;
}

void JobsPool::stop()
{
    for(auto& job : m_pool)
        job->stop();

    stateChanged(m_pool.back().get());
}

void JobsPool::pause()
{
    for(auto& job : m_pool)
        job->pause();

    stateChanged(m_pool.back().get());
}

void JobsPool::resume()
{
    for(auto& job : m_pool)
        job->resume();

    stateChanged(m_pool.back().get());
}

void JobsPool::work(JobCallback cb)
{
    for(auto& job : m_pool)
        job->work(cb);

    stateChanged(m_pool.back().get());
}

} // namespace REDasm
