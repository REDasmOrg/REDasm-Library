#pragma once

#include <unordered_set>
#include <unordered_map>
#include <array>
#include <vector>
#include <rdapi/types.h>
#include "algorithm/algorithm.h"
#include "../support/safe_ptr.h"
#include "../object.h"

class Analyzer;
class Context;

class Engine: public Object
{
    public:
        enum Steps { State_Stop, State_Algorithm, State_CFG, State_Analyze, State_Done, State_Last };

    public:
        Engine(Context* ctx);
        ~Engine();
        size_t currentStep() const;
        void reset();
        void execute();
        void execute(size_t step);
        bool cfg(rd_address address);
        void setWeak(bool b);
        bool isWeak() const;
        bool busy() const;
        void stop();

    private:
        void algorithmStep();
        void analyzeStep();
        void cfgStep();

    private:
        SafeAlgorithm& algorithm();
        void analyzeAll();
        void mergeCode();
        void generateCfg(rd_address address);
        void notifyStatus();
        void notifyBusy(bool busy);
        void setStep(size_t step);
        void nextStep();

    private:
        RDAnalysisStatus m_status{ };
        std::vector<const char*> m_analyzersnames;
        std::vector<size_t> m_analyzersdone;
        size_t m_lastnotifystep{State_Last};
        bool m_isweak{false};

    private:
        std::unordered_set<size_t> m_stepsdone;
        std::unordered_set<size_t> m_merged;

    private:
        static const std::array<const char*, State_Last> STATUS_LIST;
};

