#pragma once

#include "../../support/event.h"
#include "../../pimpl.h"
#include "job.h"

namespace REDasm {

class JobsPoolImpl;

class JobsPool: public Object
{
    REDASM_OBJECT(JobsPool)
    PIMPL_DECLARE_P(JobsPool)
    PIMPL_DECLARE_PRIVATE(JobsPool)

    public:
        Event stateChanged;

    public:
        JobsPool();
        size_t concurrency() const;
        size_t activeCount() const;
        JobState state() const;
        bool active() const;
        void stop();
        void pause();
        void resume();
        void work(const Job::JobCallback &cb);
};

} // namespace REDasm
