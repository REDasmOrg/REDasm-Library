#pragma once

#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <functional>
#include <redasm/plugins/assembler/algorithm/state.h>

namespace REDasm {

class StateMachine
{
    public:
        StateMachine();
        virtual ~StateMachine() = default;
        size_t pending() const;
        bool hasNext() const;
        void next();

    protected:
        void registerState(state_t id, const StateCallback& cb);
        void enqueueState(const State& state);
        void executeState(const State& state);
        void executeState(const State* state);
        virtual bool validateState(const State& state) const;
        virtual void onNewState(const State *state) const;

    private:
        bool getNext(State* state);

    protected:
        std::unordered_map<state_t, StateCallback> m_states;

    private:
        std::forward_list<State> m_pending;
        size_t m_count;
};

} // namespace REDasm
