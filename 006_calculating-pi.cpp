//006_calculating-pi.cpp
#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>
#include <mutex>
#include <future>
using namespace std;

double calculate_pi(int terms)
{
    double sum = 0.0;
    for(int i = 0; i < terms; i++)
    {
        int sign = pow(-1, i);
        double term = 1.0 / (i * 2 + 1);
        sum += sign * term;
    }

    return sum * 4;
}

int main()
{
    promise<double> pi_promise;
    auto do_calculate_pi = [&pi_promise](int terms) {
        double pi_value = calculate_pi(terms);
        pi_promise.set_value(pi_value);
    };

    thread t1(do_calculate_pi, 1E7);
    future<double> pi_future = pi_promise.get_future();
    t1.join();

    cout << setprecision(15) << pi_future.get() << endl;
    return 0;
}