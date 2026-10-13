//009_waiting-threads.cpp
//program to demonstrate how to wait for a thread to finish using condition_variable and mutex
// naive way using sleep_for is not recommended as it wastes CPU cycles and can lead to race conditions
// better way is to use condition_variable and mutex to synchronize threads
// cleaner version is to use condition_variable with lambda function to wait for the condition to be met
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
using namespace std;

int main()
{
    condition_variable condition, condition2; // condition variable to synchronize threads
    mutex mtx, mtx2; // mutex to protect shared variable
    bool ready = false;
    bool ready2 = false;

    // print the initial values of ready and ready2
    cout << "ready = " << ready << ", ready2 = " << ready2 << endl;

    thread t1([&]() // lambda function to run in thread 1
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        ready = true;
        lock.unlock();
        cout << "Thread 1 is running" << endl;
        condition.notify_one(); // notify the waiting thread that the condition has changed
    });

    thread t2([&]() // lambda function to run in thread 2
    {
        this_thread::sleep_for(chrono::milliseconds(2000));
        unique_lock<mutex> lock2(mtx2); // lock the mutex before changing the shared variable
        ready2 = true;
        lock2.unlock();
        cout << "Thread 2 is running" << endl;
        condition2.notify_one(); // notify the waiting thread that the condition has changed
    });

    /*
    // naive way using sleep_for
    while (!ready) 
    {
        cout << "Waiting for thread 1 to finish..." << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    while (!ready2) 
    {
        cout << "Waiting for thread 2 to finish..." << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    */

    /*
    // better way using condition_variable
    unique_lock<mutex> lock(mtx); // lock the mutex before waiting
    while (!ready) // wait until the condition variable is notified
    {
        cout << "Waiting for thread 1 to finish..." << endl;
        condition.wait(lock); // wait for the condition variable to be notified
    }
    lock.unlock();
    unique_lock<mutex> lock2(mtx2);
    while (!ready2) // wait until the condition variable is notified
    {
        cout << "Waiting for thread 2 to finish..." << endl;
        condition2.wait(lock2); // wait for the condition variable to be notified
    }
    lock2.unlock();
    */

    // cleaner version using condition_variable with lambda function
    unique_lock<mutex> lock(mtx);
    condition.wait(lock, [&]() { return ready; });
    lock.unlock();

    unique_lock<mutex> lock2(mtx2);
    condition2.wait(lock2, [&]() { return ready2; });
    lock2.unlock();

    t1.join(); // wait for thread 1 to finish
    t2.join(); // wait for thread 2 to finish

    // print the final values of ready and ready2
    cout << "ready = " << ready << ", ready2 = " << ready2 << endl;
    
    return 0;
}