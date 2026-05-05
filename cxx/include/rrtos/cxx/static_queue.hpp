#ifndef RRTOS_CXX_STATIC_QUEUE_HPP
#define RRTOS_CXX_STATIC_QUEUE_HPP

#include <stddef.h>

#include "etl/queue.h"

namespace rrtos {

template <typename T, size_t Capacity>
class StaticQueue {
public:
    bool push(const T &value)
    {
        if (queue_.full()) {
            return false;
        }
        queue_.push(value);
        return true;
    }

    bool pop(T &value)
    {
        if (queue_.empty()) {
            return false;
        }
        value = queue_.front();
        queue_.pop();
        return true;
    }

    bool empty() const
    {
        return queue_.empty();
    }

    bool full() const
    {
        return queue_.full();
    }

    size_t size() const
    {
        return queue_.size();
    }

    static constexpr size_t capacity()
    {
        return Capacity;
    }

private:
    etl::queue<T, Capacity> queue_;
};

} // namespace rrtos

#endif
