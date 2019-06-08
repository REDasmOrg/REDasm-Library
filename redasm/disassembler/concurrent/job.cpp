#include "job.h"
#include <impl/disassembler/concurrent/job_impl.h>

namespace REDasm {

Job::Job(): m_pimpl_p(new JobImpl(this)) { }
JobState Job::state() const { PIMPL_P(const Job); return p->state(); }
size_t Job::id() const { PIMPL_P(const Job); return p->id(); }
bool Job::active() const { PIMPL_P(const Job); return p->active(); }
void Job::start() { PIMPL_P(Job); p->start(); }
void Job::stop() { PIMPL_P(Job); p->stop(); }
void Job::pause() { PIMPL_P(Job); p->pause(); }
void Job::resume() { PIMPL_P(Job); p->resume(); }
void Job::setOneShot(bool b) { PIMPL_P(Job); p->setOneShot(b); }
void Job::work(const JobCallback& cb, bool deferred) { PIMPL_P(Job); p->work(cb, deferred); }

} // namespace REDasm
