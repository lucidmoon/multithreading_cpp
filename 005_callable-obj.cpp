// 005_callable-obj.cpp

#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

class App
{
    int count = 0;
    mutex mtx;

    public:
        void operator()()
        {
            cout << "Hello from callable object" << endl;
            for (int i = 0; i < 1E5; ++i) {
            //mtx.lock();
            lock_guard<mutex> lock(mtx);
            count++;
            //mtx.unlock();
            }
        }

        int getCount() const
        {
            return count;
        }
};


int main(){

    App app;
    //app(); // call the callable object
    

    thread t1(ref(app));
    thread t2(ref(app));


    t1.join();
    t2.join();
 

    cout << app.getCount() << endl;
    return 0;
}