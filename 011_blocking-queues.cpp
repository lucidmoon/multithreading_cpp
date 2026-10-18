// 011_blocking-queues.cpp
// A thread-safe bounded queue that blocks on full/empty conditions.
// idea is so the   queue can be accessible with multple thread without worry about race conditions or data corruption
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

using namespace std;

template <typename E>
class blocking_queue
{
    using queue_type = queue<E>; // making alias queue_type for std::queue<E>
    queue_type _queue; //actual storage container
    const size_t _max_size; //max capacity of the queue
    mutable mutex _mutex; //lock to running thread (queue) one at a time
    condition_variable _not_empty;
    condition_variable _not_full;

public:
    explicit blocking_queue(size_t max_size)
        : _max_size(max_size)
    {
    }

    void push(const E& value)
    {
        unique_lock<mutex> lock(_mutex);
        _not_full.wait(lock, [this]() { return _queue.size() < _max_size; });
        _queue.push(value);
        lock.unlock();
        _not_empty.notify_one();
    }

    void push(E&& value)
    {
        unique_lock<mutex> lock(_mutex);
        _not_full.wait(lock, [this]() { return _queue.size() < _max_size; });
        _queue.push(move(value));
        lock.unlock();
        _not_empty.notify_one();
    }

    E pop()
    {
        unique_lock<mutex> lock(_mutex);
        _not_empty.wait(lock, [this]() { return !_queue.empty(); });

        E item = move(_queue.front());
        _queue.pop();

        lock.unlock();
        _not_full.notify_one();
        return item;
    }

    bool try_push(const E& value)
    {
        lock_guard<mutex> lock(_mutex);
        if (_queue.size() >= _max_size)
        {
            return false;
        }

        _queue.push(value);
        _not_empty.notify_one();
        return true;
    }

    bool try_pop(E& value)
    {
        lock_guard<mutex> lock(_mutex);
        if (_queue.empty())
        {
            return false;
        }

        value = move(_queue.front());
        _queue.pop();
        _not_full.notify_one();
        return true;
    }
};

int main()
{
    blocking_queue<int> queue(5);

    thread producer([&]() {
        for (int i = 0; i < 20; ++i)
        {
            queue.push(i);
            cout << "produced " << i << endl;
        }
    });

    thread consumer([&]() {
        for (int i = 0; i < 20; ++i)
        {
            int item = queue.pop();
            cout << "consumed " << item << endl;
        }
    });

    producer.join();
    consumer.join();

    return 0;
}