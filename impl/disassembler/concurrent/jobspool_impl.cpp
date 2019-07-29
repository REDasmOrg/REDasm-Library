#include "jobspool_impl.h"
#include <redasm/context.h>
#include <algorithm>
#include <thread>

namespace REDasm {

JobsPoolImpl::JobsPoolImpl(JobsPool *q): m_pimpl_q(q), m_running(true)
{
    m_concurrency = std::thread::hardware_concurrency();

    if(!m_concurrency || r_ctx->sync())
        m_concurrency = 1;

    for(size_t i = 0; i < m_concurrency; i++)
    {
        auto job = std::make_unique<Job>();

        job->stateChanged.connect(this, std::bind(&JobsPoolImpl::notifyStateChanged, this, std::placeholders::_1));
        m_jobs.push_back(std::move(job));
    }
}

JobsPoolImpl::~JobsPoolImpl() { m_running = false; }

size_t JobsPoolImpl::concurrency() const
{
    return m_jobs.size();
}

size_t JobsPoolImpl::activeCount() const
{
    size_t i = 0;

    for(auto& job : m_jobs)
    {
        if(job->active())
            i++;
    }

    return i;
}

JobState JobsPoolImpl::state() const
{
    JobState s = JobState::InactiveState;

    for(auto& job : m_jobs)
    {
        if(job->state() == JobState::InactiveState)
            continue;

        s = job->state();

        if(job->state() != JobState::ActiveState)
            break;
    }

    return s;
}

bool JobsPoolImpl::active() const
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

void JobsPoolImpl::stop()
{
    for(auto& job : m_jobs)
        job->stop();

    PIMPL_Q(JobsPool);
    q->stateChanged(m_jobs.back().get());
}

void JobsPoolImpl::pause()
{
    for(auto& job : m_jobs)
        job->pause();

    PIMPL_Q(JobsPool);
    q->stateChanged(m_jobs.back().get());
}

void JobsPoolImpl::resume()
{
    for(auto& job : m_jobs)
        job->resume();

    PIMPL_Q(JobsPool);
    q->stateChanged(m_jobs.back().get());
}

void JobsPoolImpl::work(const Job::JobCallback &cb)
{
    for(auto& job : m_jobs)
        job->work(cb);

    PIMPL_Q(JobsPool);
    q->stateChanged(m_jobs.back().get());
}

void JobsPoolImpl::notifyStateChanged(EventArgs *e)
{
    Job* job = variant_object<Job>(e->arg());

    for(const auto& j : m_jobs)
    {
        if(job->state() != j->state())
            return;
    }

    PIMPL_Q(JobsPool);
    q->stateChanged(job);
}

} // namespace REDasm
