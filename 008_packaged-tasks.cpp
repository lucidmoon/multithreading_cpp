//008_packaged-tasks.cpp
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
    packaged_task<double(int)> task1(calculate_pi); // Create a packaged task that wraps the calculate_pi function
    future<double> pi_future = task1.get_future(); // Get a future associated with the packaged task
    
    thread t1(move(task1), 1E6); // Move the packaged task into a new thread and call it with 1 million terms
    //thread t1(move(task1), 0); // Move the packaged task into a new thread and call it with 0 terms to trigger an exception 
    try
    {
        double result = pi_future.get(); // Wait for the result from the packaged task
        cout << setprecision(15) << result << endl; // Print the result with high precision

    }
    catch(const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
    }
    t1.join(); // Wait for the thread to complete
    return 0;
}