#include "job.h"
#include "../../redasm_runtime.h"

namespace REDasm {

size_t Job::m_jobid = 0;

Job::Job(): m_id(++m_jobid), m_state(Job::InactiveState) { }

Job::~Job()
{
    m_state = Job::InactiveState;

    if(!m_thread.joinable())
        return;

    m_cv.notify_one();
    m_thread.join();
}

size_t Job::state() const { return m_state; }
size_t Job::id() const { return m_id; }
bool Job::active() const { return m_state > Job::SleepState; }

void Job::start()
{
    if(m_state == Job::ActiveState)
        return;

    m_state = Job::ActiveState;

    if(REDasm::Runtime::sync())
        this->doWorkSync();
    else
        m_cv.notify_one();
}

void Job::stop()
{
    if(m_state <= Job::SleepState)
        return;

    m_state = Job::SleepState;
    stateChanged(this);
}

void Job::pause()
{
    if(!this->active() || (m_state != Job::ActiveState))
        return;

    m_state = Job::PausedState;
    stateChanged(this);
}

void Job::resume()
{
    if(!this->active() || (m_state != Job::PausedState))
        return;

    m_state = Job::ActiveState;
    stateChanged(this);
}

void Job::work(JobCallback cb, bool deferred)
{
    if(this->active())
        return;

    m_state = deferred ? Job::SleepState : Job::ActiveState;
    m_jobcallback = cb;
    stateChanged(this);

    if(REDasm::Runtime::sync())
    {
        this->doWorkSync();
        return;
    }

    if(m_thread.joinable())
        m_cv.notify_one();
    else
        m_thread = std::thread(&Job::doWork, this);
}

void Job::doWork()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    for( ; ; )
    {
        while((m_state == Job::SleepState) || (m_state == Job::PausedState))
            m_cv.wait(lock);

        if(m_state == Job::InactiveState)
            return;

        if(m_state == Job::ActiveState)
            m_jobcallback(this);
    }
}

void Job::doWorkSync()
{
    while(this->active())
    {
        if(m_state == Job::ActiveState)
            m_jobcallback(this);
    }
}

} // namespace REDasm
