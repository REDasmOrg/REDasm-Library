#pragma once

#include <mutex>
#include <array>

template<typename T, size_t capacity>
class RingBuffer
{
    private:
        using ring_lock = std::scoped_lock<std::mutex>;

    public:
        bool push_back(const T& item) {
            ring_lock lock(m_mutex);
            size_t next = (m_head + 1) % capacity;
            if(next == m_tail) return false;

            m_data[m_head] = item;
            m_head = next;
            return true;
        }

        bool pop_front(T& item) {
            ring_lock lock(m_mutex);
            if(m_tail == m_head) return false;

            item = m_data[m_tail];
            m_tail = (m_tail + 1) % capacity;
            return true;
        }

        void reset() {
            ring_lock lock(m_mutex);
            m_data.fill(0);
            m_head = m_tail = 0;
        }

    private:
        std::array<T, capacity> m_data;
        size_t m_head{0}, m_tail{0};
        std::mutex m_mutex;
};
