#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <unordered_map>
#include <functional>

template<typename KEY, typename RET, typename ...ARGS> class ValuedDispatcher: protected std::unordered_map< KEY, std::function<RET(ARGS...)> >
{
    public:
        typedef std::function<RET(ARGS...)> DispatcherType;
        typedef std::unordered_map<KEY, DispatcherType> Type;

    public:
        using Type::operator[];
        using Type::begin;
        using Type::end;
        using Type::empty;
        using Type::size;
        using Type::find;

    public:
        ValuedDispatcher(): Type() { }
        bool contains(KEY key) const { return this->find(key) != this->end(); }

        RET operator()(KEY key, ARGS... args) {
            auto it = this->find(key);

            if(it != this->end())
                return it->second(std::forward<ARGS>(args)...);

            return RET();
        }
};

template<typename KEY, typename ...ARGS> using Dispatcher = ValuedDispatcher<KEY, void, ARGS...>;

#endif // DISPATCHER_H
