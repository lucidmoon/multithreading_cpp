//007_promise-exception.cpp
#include <iostream>
#include <cmath>
#include <thread>
#include <iomanip>
#include <future>
#include <exception>
using namespace std;

double calculate_pi(int terms)
{
    double sum = 0.0;

    if(terms <= 0)
    {
        throw invalid_argument("Number of terms must be positive.");
    }
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
        try
        {
            auto pi_value = calculate_pi(terms); // This will throw an exception if terms <= 0
            pi_promise.set_value(pi_value);
        }
        catch(...)
        {
            pi_promise.set_exception(current_exception());
        }
    };

    thread t1(do_calculate_pi, 0); // This will throw an exception due to invalid argument
    //thread t1(do_calculate_pi, 1E7);
    future<double> pi_future = pi_promise.get_future();
    try
    {
        cout << setprecision(15) << pi_future.get() << endl;
    }
    catch(const exception& e)
    {
        cout << "Exception caught: " << e.what() << endl;
    }
    
    t1.join();
    return 0;
}