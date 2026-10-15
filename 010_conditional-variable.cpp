#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

int main()
{
    bool ready = false; // shared variable to indicate if the thread is ready
    bool ready2 = false; // shared variable to indicate if the thread is ready
    mutex mtx; // mutex to protect the shared variable
    condition_variable condition, condition2; // condition variable to synchronize threads

    thread t1 ([&]() // lambda function to run in thread 1
    {
        this_thread::sleep_for(chrono::milliseconds(1000)); // simulate some work
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        ready = true; // set the shared variable to true
        cout << "Thread 1 is running" << endl;
        condition.notify_one(); // notify the waiting thread that the condition has changed
    });

    thread t2 ([&]() // lambda function to run in thread 2
    {
        this_thread::sleep_for(chrono::milliseconds(2000)); // simulate some work
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        ready2 = true; // set the shared variable to true
        cout << "Thread 2 is running" << endl;
        condition2.notify_one(); // notify the waiting thread that the condition has changed
    });


    unique_lock<mutex> lock(mtx); // lock the mutex before checking the shared variable
    condition.wait(lock, [&ready] { return ready; }); // condition variable to synchronize threads
    lock.unlock(); // unlock the mutex before waiting for the condition variable

    unique_lock<mutex> lock2(mtx); // lock the mutex before waiting for the condition variable
    condition2.wait(lock2, [&ready2] { return ready2; }); // wait for the condition variable to be notified and the shared variable to be true
    lock2.unlock(); // unlock the mutex after waiting for the condition variable

    cout << "Threads have finished" << endl;
    
    // wait for the threads to finish
    t1.join();
    t2.join();

    return 0;
}