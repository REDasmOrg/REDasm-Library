#pragma once

#include "../../support/event.h"
#include "../../pimpl.h"
#include "jobstate.h"

namespace REDasm {

class JobImpl;
class Job;

class Job: public Object
{
    REDASM_OBJECT(Job)
    PIMPL_DECLARE_P(Job)
    PIMPL_DECLARE_PRIVATE(Job)

    public:
        typedef std::function<void(Job*)> JobCallback;
        Event stateChanged;

    public:
        Job();
        JobState state() const;
        size_t id() const;
        bool active() const;
        void start();
        void stop();
        void pause();
        void resume();
        void setOneShot(bool b);
        void work(const JobCallback &cb, bool deferred = false);
};

} // namespace REDasm
