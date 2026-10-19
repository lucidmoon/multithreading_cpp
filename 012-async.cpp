// 012_async.cpp
// demonstrates the use of std::async to run tasks asynchronously in C++17.
// The async function is used to launch a task in a separate thread and returns a future object 
// that can be used to retrieve the result of the task once it has completed..

#include <iostream>
#include <future>
#include <chrono>

using namespace std;

int work(int id)
{
    for (int i = 0; i < 5; i++)
    {
        cout << "running " << id << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    return id * 7;
}

int main()
{
    future<int> f1 = async(launch::async, work, 0);
    future<int> f2 = async(launch::async, work, 1);

    cout << f1.get() << endl;
    cout << f2.get() << endl;
    return 0;
}