#pragma once

#include <memory>
#include <list>
#include <redasm/disassembler/concurrent/jobspool.h>
#include <redasm/redasm.h>

namespace REDasm {

class JobsPoolImpl
{
    PIMPL_DECLARE_Q(JobsPool)
    PIMPL_DECLARE_PUBLIC(JobsPool)

    public:
        JobsPoolImpl(JobsPool* q);
        ~JobsPoolImpl();
        size_t concurrency() const;
        size_t activeCount() const;
        JobState state() const;
        bool active() const;
        void stop();
        void pause();
        void resume();
        void work(const Job::JobCallback &cb);

    private:
        void notifyStateChanged(EventArgs* e);

    private:
        std::list< std::unique_ptr<Job> > m_jobs;
        size_t m_concurrency;
        bool m_running;
};

} // namespace REDasm
