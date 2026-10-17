//010_conditional-variable
// cleaner way from 009 using conditional_variable
// Why it's cleaner? because it only use 1 mutex being re-locked (no wasted memory) since we use block scope { } that force lock to clean up via RAII (Resources Acquisition Is Initialization)
// and have timed polling (300ms) status checking instead of indefinite blocking in 009
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

int main()
{
    bool ready = false; // shared variable to indicate if the thread is ready
    bool ready2 = false; // shared variable to indicate if the thread is ready
    mutex mtx; // mutex to protect the shared variable
    condition_variable condition, condition2; // condition variable to synchronize threads

    // print the initial values of ready and ready2
    cout << "ready = " << ready << ", ready2 = " << ready2 << endl;

    thread t1 ([&]() // lambda function to run in thread 1
    {
        this_thread::sleep_for(chrono::milliseconds(1000)); // simulate some work
        cout << "t1 acquiring lock" << endl;
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        cout << "t1 acquired lock" << endl;

        cout << "t1 intend to releasing lock, notifying..." << endl;
        ready = true; // set the shared variable to true
        //cout << "Thread 1 is running" << endl;
        condition.notify_one(); // notify the waiting thread that the condition has changed
    });

    thread t2 ([&]() // lambda function to run in thread 2
    {
        this_thread::sleep_for(chrono::milliseconds(2000)); // simulate some work
        cout << "t2 acquiring lock" << endl;
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        cout << "t2 acquired lock" << endl;

        cout << "t1 intend to releasing lock, notifying..." << endl;
        ready2 = true; // set the shared variable to true
        //cout << "Thread 2 is running" << endl;
        condition2.notify_one(); // notify the waiting thread that the condition has changed
    });

    //cleaner way
    {
        cout << "main thread acquiring lock" << endl;
        unique_lock<mutex> lock(mtx); // lock the mutex before checking the shared variable
        cout << "main thread acquired lock, waiting..." << endl;
        while(!condition.wait_for(lock, chrono::milliseconds(300), [&ready] { return ready; })) // condition variable to synchronize threads
        {
            cout << "Waiting for thread 1 to finish..." << endl;
        }
        cout << "t1 releasing lock" << endl;
        lock.unlock(); // unlock the mutex before waiting for the condition variable
    }
    cout << "main thread t1 finished waiting" << endl;
    cout << "ready" << endl;

    {
        cout << "main thread acquiring lock" << endl;
        unique_lock<mutex> lock2(mtx); // lock the mutex before waiting for the condition variable
        cout << "main thread acquired lock, waiting..." << endl;
        while(!condition2.wait_for(lock2, chrono::milliseconds(300), [&ready2] { return ready2; })) // wait for the condition variable to be notified and the shared variable to be true
        {
            cout << "Waiting for thread 2 to finish..." << endl;
        }
        cout << "t2 releasing lock" << endl;
        lock2.unlock(); // unlock the mutex after waiting for the condition variable
    }
    cout << "main thread t2 finished waiting" << endl;
    cout << "ready" << endl;
    
    // wait for the threads to finish
    t1.join();
    t2.join();

    // print the final values of ready and ready2
    //cout << "ready = " << ready << ", ready2 = " << ready2 << endl;
    cout << "Threads have finished" << endl;

    return 0;
}