#pragma once

// Based on: https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/

#include <functional>

namespace REDasm {

struct JobDispatchArgs
{
    size_t jobIndex;
    size_t groupIndex;
};

class JobManager
{
    public:
        typedef std::function<void()> JobCallback;
        typedef std::function<void(const JobDispatchArgs*)> JobDispatchCallback;

    public:
        JobManager() = delete;
        static size_t concurrency();
        static bool last();
        static bool busy();
        static void wait();
        static void dispatch(const JobDispatchCallback& cb);
        static void dispatch(size_t jobcount, const JobDispatchCallback& cb);
        static void dispatch(size_t jobcount, size_t groupsize, const JobDispatchCallback& cb);
        static void execute(const JobCallback& cb);
        static void deinitialize();
        static void initialize();

    public:
        template<typename Class, typename Member> static void execute(Class* c, const Member& m) {
            JobManager::execute(std::bind(m, c));
        }

        template<typename Class, typename Member> static void dispatch(Class* c, const Member& m) {
            JobManager::dispatch(std::bind(m, c, std::placeholders::_1));
        }

        template<typename Class, typename Member> static void dispatch(size_t jobcount, Class* c, const Member& m) {
            JobManager::dispatch(jobcount, std::bind(m, c, std::placeholders::_1));
        }

        template<typename Class, typename Member> static void dispatch(size_t jobcount, size_t groupsize, Class* c, const Member& m) {
            JobManager::dispatch(jobcount, groupsize, std::bind(m, c, std::placeholders::_1));
        }
};

} // namespace REDasm

