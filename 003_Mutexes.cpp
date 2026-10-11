#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

int main(){
    int count = 0;
    const int ITERS = 1E7;
    mutex mtx;

    auto func = [&](){ // Lambda function to increment count
        for (int i = 0; i < ITERS; ++i) {
            //mtx.lock();
            lock_guard<mutex> lock(mtx);
            count++;
            //mtx.unlock();
        }
    };
    thread t1(func);
    thread t2(func);

    t1.join();
    t2.join();

    cout << count << endl;
    return 0;
}