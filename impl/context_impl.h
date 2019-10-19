#pragma once

#define CONTEXT_DEBOUNCE_TIMEOUT_MS 100

#include <redasm/context.h>
#include <chrono>
#include <set>

namespace REDasm {

class ContextImpl
{
    PIMPL_DECLARE_Q(Context)
    PIMPL_DECLARE_PUBLIC(Context)

    public:
        ContextImpl();
        Disassembler* disassembler() const;
        void setDisassembler(Disassembler* disassembler);
        void checkSettings();
        void flag(ContextFlags flag, bool set);
        bool hasFlag(ContextFlags flag) const;
        String capstoneVersion() const;

    private:
        ContextSettings m_settings;
        std::chrono::steady_clock::time_point m_laststatusreport;
        std::chrono::milliseconds m_debouncetimeout;
        std::set<String> m_uproblems;
        ProblemList m_problems;

    private:
        Disassembler* m_disassembler{nullptr};
        ContextFlags m_flags{ContextFlags::None};

    private:
        static std::unique_ptr<Context> m_instance;
        static Context* m_parentinstance;
};

} // namespace REDasm
