//009_waiting-threads.cpp
//program to demonstrate how to wait for a thread to finish using condition_variable and mutex
// naive way using sleep_for is not recommended as it wastes CPU cycles and can lead to race conditions
// better way is to use condition_variable and mutex to synchronize threads
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

int main()
{
    condition_variable condition; // condition variable to synchronize threads
    mutex mtx;
    bool ready = false;
    bool ready2 = false;
    
    thread t1([&]() // lambda function to run in thread 1
    {
        this_thread::sleep_for(chrono::milliseconds(2000));
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        ready = true;
        lock.unlock();
        cout << "Thread 1 is running" << endl;
        condition.notify_one(); // notify the waiting thread that the condition has changed
    });

    thread t2([&]() // lambda function to run in thread 2
    {
        this_thread::sleep_for(chrono::milliseconds(2000));
        unique_lock<mutex> lock(mtx); // lock the mutex before changing the shared variable
        ready2 = true;
        lock.unlock();
        cout << "Thread 2 is running" << endl;
        condition.notify_one(); // notify the waiting thread that the condition has changed
    });

    /*
    // naive way using sleep_for
    while (!ready) 
    {
        cout << "Waiting for thread 1 to finish..." << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    */

    // better way using condition_variable
    unique_lock<mutex> lock(mtx); // lock the mutex before waiting
    while (!ready) // wait until the condition variable is notified
    {
        cout << "Waiting for thread 1 to finish..." << endl;
        condition.wait(lock); // wait for the condition variable to be notified
    }
    unique_lock<mutex> lock2(mtx); // lock the mutex before waiting
    while (!ready) // wait until the condition variable is notified
    {
        cout << "Waiting for thread 2 to finish..." << endl;
        condition.wait(lock2); // wait for the condition variable to be notified
    }
    
    t1.join();
    t2.join();
    cout << "ready = " << ready << endl;
    return 0;
}