// 004_function-guard.cpp

#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

// using ref() -> preferred, cleaner
void work(int &count, mutex &mtx)
{
    for (int i = 0; i < 1E5; ++i) {
            //mtx.lock();
            lock_guard<mutex> lock(mtx);
            count++;
            //mtx.unlock();
        }
}

// using pointers -> must handle dereferencing and null pointers
void work_ptr(int *count, mutex *mtx)
{
    for (int i = 0; i < 1E5; ++i) {
            //mtx->lock();
            lock_guard<mutex> lock(*mtx);
            (*count)++;
            //mtx->unlock();
        }
}
int main(){
    int count = 0;
    mutex mtx;

    thread t1(work, ref(count), ref(mtx));
    thread t2(work, ref(count), ref(mtx));

    thread t3(work_ptr, &count, &mtx);
    thread t4(work_ptr, &count, &mtx);
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    cout << count << endl;
    return 0;
}