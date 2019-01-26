#ifndef JOBSPOOL_H
#define JOBSPOOL_H

#include <list>
#include "../../redasm_api.h"
#include "../event.h"
#include "job.h"

namespace REDasm {

class JobsPool
{
    public:
        Event<Job*> stateChanged;

    public:
        JobsPool();
        ~JobsPool();
        size_t concurrency() const;
        size_t activeCount() const;
        size_t state() const;
        bool active() const;
        void stop();
        void pause();
        void resume();
        void work(JobCallback cb);

    private:
        void notifyStateChanged(Job*job);

    private:
        std::list< std::unique_ptr<Job> > m_jobs;
        size_t m_concurrency;
        bool m_running;
};

} // namespace REDasm

#endif // JOBSPOOL_H
