// 015_thread-pool.cpp
// This program demonstrates how to use a thread pool to execute multiple tasks concurrently. 
// It defines a ThreadPool class that manages a set of threads and allows tasks to be submitted for execution. 
// Each task is represented by a Callable object, which can be either a function or a lambda expression. 
// The main function submits several tasks to the thread pool and waits for them to complete before exiting.

#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>
#include <queue>

using namespace std;

mutex g_mtx;

// This template class implements a thread-safe queue using mutex and condition variable.
template <typename E>
class blocking_queue
{
private:
    // Mutex to protect the queue from concurrent access.
    mutex _mtx;
    // Condition variable to signal when the queue is not empty or full.
    condition_variable _cond;
    // Maximum size of the queue.
    int _max_size;
    // Queue to store elements.
    queue<E> _queue;

public:
    // Constructor initializes maximum size of the queue.
    blocking_queue(int max_size) : _max_size(max_size)
    {
    }

    // Function to add an element to the queue. If the queue is full, it waits until space becomes available.
    void push(E e)
    {
        unique_lock<mutex> lock(_mtx);

        _cond.wait(lock, [this]() { return _queue.size() < _max_size; });

        _queue.push(e);

        lock.unlock();
        _cond.notify_one();
    }

    // Function to retrieve the front element of the queue. If the queue is empty, it waits until an element becomes available.
    E front()
    {
        unique_lock<mutex> lock(_mtx);
        _cond.wait(lock, [this]() { return !_queue.empty(); });

        return _queue.front();
    }

    // Function to remove the front element of the queue. If the queue is empty, it waits until an element becomes available.
    void pop()
    {
        unique_lock<mutex> lock(_mtx);
        _cond.wait(lock, [this]() { return !_queue.empty(); });

        _queue.pop();

        lock.unlock();
        _cond.notify_one();
    }

    // Function to get the size of the queue.
    int size()
    {
        lock_guard<mutex> lock(_mtx);
        return _queue.size();
    }
};

int work(int id)
{
    // Lock the global mutex before printing and sleeping to avoid race conditions.
    unique_lock<mutex> lock(g_mtx);
    cout << "Starting " << id << endl;
    lock.unlock();

    // Generate a random sleep time between 3 and 8 seconds.
    int seconds = int((5.0 * rand()) / RAND_MAX + 3);
    this_thread::sleep_for(chrono::seconds(seconds));

    return id;
}

int main()
{
    // Create a blocking queue with a maximum size of 2, which is one less than the number of threads we want to launch.
    blocking_queue<shared_future<int>> futures(2);

    // Launch a thread that will generate work items and add them to the queue.
    thread t([&]() {
        for (int i = 0; i < 20; i++)
        {
            // Asynchronously call the work function and store the result in a shared future object.
            shared_future<int> f = async(launch::async, work, i);
            futures.push(f); // Add the shared future to the queue.
        }
    });

    // Consume the work items from the queue and print their results.
    for (int i = 0; i < 20; i++)
    {
        shared_future<int> f = futures.front(); // Get the front shared future object.
        int value = f.get(); // Retrieve the result of the asynchronous function call.
        futures.pop(); // Remove the shared future from the queue.
        cout << "Returned: " << value << endl;
    }

    // Join the thread that generated work items to wait for it to complete before exiting the main function.
    t.join();

    return 0;
}