#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

int main()
{
    atomic<int> count = 0;
    const int ITERS = 1E6;

    thread t1([&count, ITERS](){
        for (int i = 0; i < ITERS; ++i)
        {
            count++;
        }
    });

    thread t2([&count, ITERS](){
        for (int i = 0; i < ITERS; ++i)
        {
            count++;
        }
    });

    t1.join();
    t2.join();

    cout << count << endl;
    return 0;
}