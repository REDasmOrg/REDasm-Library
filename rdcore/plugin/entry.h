#pragma once

#include "../object.h"

struct RDEntry;

template<typename T>
class Entry: public Object
{
    public:
        Entry(const T* e, Context* ctx): Object(ctx), m_entry(e) { }
        const T* plugin() const { return m_entry; }
        const char* id() const { return m_entry->id; }
        const char* name() const { return m_entry->name; }

    protected:
        const T* m_entry;
};
