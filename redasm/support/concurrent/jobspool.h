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
        size_t concurrency() const;
        size_t activeCount() const;
        size_t state() const;
        bool active() const;
        void stop();
        void pause();
        void resume();
        void work(JobCallback cb);

    private:
        std::list< std::unique_ptr<Job> > m_pool;
        size_t m_concurrency;
};

} // namespace REDasm

#endif // JOBSPOOL_H
