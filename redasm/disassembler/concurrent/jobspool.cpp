#include "jobspool.h"
#include <impl/disassembler/concurrent/jobspool_impl.h>

namespace REDasm {

JobsPool::JobsPool(): m_pimpl_p(new JobsPoolImpl(this)) { }
size_t JobsPool::concurrency() const { PIMPL_P(const JobsPool); return p->concurrency(); }
size_t JobsPool::activeCount() const { PIMPL_P(const JobsPool); return p->activeCount(); }
JobState JobsPool::state() const { PIMPL_P(const JobsPool); return p->state(); }
bool JobsPool::active() const { PIMPL_P(const JobsPool); return p->active(); }
void JobsPool::stop() { PIMPL_P(JobsPool); p->stop(); }
void JobsPool::pause() { PIMPL_P(JobsPool); p->pause(); }
void JobsPool::resume() { PIMPL_P(JobsPool); p->resume(); }
void JobsPool::work(const JobCallback& cb) { PIMPL_P(JobsPool); p->work(cb); }

} // namespace REDasm
