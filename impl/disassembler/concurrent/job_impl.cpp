#include "job_impl.h"
#include <redasm/context.h>
#include <algorithm>

#define JOB_BASE_INTERVAL 1 // 1ms
#define JOB_MAX_INTERVAL  5 // 5ms

namespace REDasm {

size_t JobImpl::m_jobid = 0;

JobImpl::JobImpl(Job *q): m_pimpl_q(q), m_oneshot(false), m_state(JobState::InactiveState), m_interval(JOB_BASE_INTERVAL), m_id(++m_jobid) { }

JobImpl::~JobImpl()
{
    m_state = JobState::InactiveState;

    if(!m_thread.joinable())
        return;

    m_cv.notify_one();
    m_thread.join();
}

JobState JobImpl::state() const { return m_state; }
size_t JobImpl::id() const { return m_id; }
bool JobImpl::active() const { return m_state > JobState::SleepState; }

void JobImpl::start()
{
    if((m_state == JobState::InactiveState) || (m_state == JobState::ActiveState))
        return;

    m_state = JobState::ActiveState;

    if(r_ctx->sync())
        this->doWorkSync();
    else
        m_cv.notify_one();
}

void JobImpl::stop()
{
    if(m_state <= JobState::SleepState)
        return;

    PIMPL_Q(Job);
    m_state = JobState::SleepState;
    q->stateChanged(q);
}

void JobImpl::pause()
{
    if(!this->active() || (m_state != JobState::ActiveState))
        return;

    PIMPL_Q(Job);
    m_state = JobState::PausedState;
    q->stateChanged(q);
}

void JobImpl::resume()
{
    if(!this->active() || (m_state != JobState::PausedState))
        return;

    PIMPL_Q(Job);
    m_state = JobState::ActiveState;
    q->stateChanged(q);
}

void JobImpl::setOneShot(bool b) { m_oneshot = b; }

void JobImpl::work(const JobCallback &cb, bool deferred)
{
    if(this->active())
        return;

    m_state = deferred ? JobState::SleepState : JobState::ActiveState;
    m_jobcallback = cb;

    PIMPL_Q(Job);
    q->stateChanged(q);

    if(r_ctx->sync())
    {
        this->doWorkSync();
        return;
    }

    if(m_thread.joinable())
        m_cv.notify_one();
    else
        m_thread = std::thread(&JobImpl::doWork, this);
}

void JobImpl::sleep()
{
    PIMPL_Q(Job);
    m_state = JobState::InactiveState;
    q->stateChanged(q);
}

void JobImpl::doWork()
{
    PIMPL_Q(Job);
    std::unique_lock<std::mutex> lock(m_mutex);

    for( ; ; )
    {
        while((m_state == JobState::SleepState) || (m_state == JobState::PausedState))
            m_cv.wait(lock);

        if(m_state == JobState::InactiveState)
            return;

        if(m_state == JobState::ActiveState)
        {
            auto start = std::chrono::steady_clock::now();
            m_jobcallback(q);

            if(m_oneshot)
            {
                this->sleep();
                return;
            }

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            m_interval = std::min(std::max((m_interval + elapsed) / 2, std::chrono::milliseconds(JOB_BASE_INTERVAL)),
                                  std::chrono::milliseconds(JOB_MAX_INTERVAL));
        }

        std::this_thread::sleep_for(m_interval);
    }
}

void JobImpl::doWorkSync()
{
    PIMPL_Q(Job);

    while(this->active())
    {
        if(m_state == JobState::ActiveState)
        {
            m_jobcallback(q);

            if(m_oneshot)
            {
                this->sleep();
                return;
            }
        }
    }
}

} // namespace REDasm
