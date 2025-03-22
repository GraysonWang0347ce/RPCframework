#ifndef BLOCKEDQUEUE_H
#define BLODKEDQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockedQ
{
public:
    void push_back(const T &);

    T pop_front();

private:
    std::queue<T> _q;
    std::mutex _mutex;
    // std::condition_variable _not_full;
    std::condition_variable _not_empty;
};

template <typename T>
void BlockedQ<T>::push_back(const T &item)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _q.push(item);
    _not_empty.notify_all();
}

template <typename T>
T BlockedQ<T>::pop_front()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _not_empty.wait(lock, [this]() -> bool
                    { return !_q.empty(); });

    T ret = _q.front();
    _q.pop();

    return ret;
}

#endif