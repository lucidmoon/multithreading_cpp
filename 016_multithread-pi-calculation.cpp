// 016_multithread-pi-calculation.cpp
// distribute work on multiple threads
#define _USE_MATH_DEFINES
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <chrono>
#include <mutex>
#include <cmath>
#include <iomanip>

using namespace std;

// very inefficient pi calculation
// Gregory-Leibniz series algorithm
double calculate_pi(int terms, int start, int skip) 
{
    double sum = 0.0;

    for (int i = start; i < terms; i += skip)
    {
        // calculate sign as (-1)^i
        int sign = pow(-1, i);
        // calculate term as 1 / (2i + 1)
        double term = 1.0 / (i * 2 + 1);
        sum += sign * term; // add the term to the sum with appropriate sign
    }
    return sum * 4; // multiply by 4 to get the final pi estimate
}

int main() {
    vector<shared_future<double>> futures;

    // determine the number of available cores and use it as the concurrency value
    const int CONCURRENCY = thread::hardware_concurrency();

    // start timer
    auto start = chrono::steady_clock::now();

    // create a shared future for each core, calculating a portion of the pi estimate
    for (int i = 0; i < CONCURRENCY; i++)
    {
        shared_future<double> f = async(launch::async, calculate_pi, 1E7, i, CONCURRENCY);
        futures.push_back(f);
    }

    double sum = 0.0;

    // get the results from each core and add them to the final pi estimate
    for (auto f : futures)
    {
        sum += f.get();
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "- With Multithread - " << endl;
    cout << setprecision(15) << "Reference PI : " << M_PI << endl; // print reference value of pi
    cout << setprecision(15) << "Calculated PI: " << sum << endl; // print calculated value of pi
    cout << "Duration: " << duration << " ms" << endl;
    cout << "============================================" << endl;

    //without multithread
    // start timer
    auto start2 = chrono::steady_clock::now();
    double sum2 = 0.0;
    for (int i = 0; i < CONCURRENCY; i++)
    {
        sum2 += calculate_pi( 1E7, i, CONCURRENCY);
    }
    auto end2 = chrono::steady_clock::now();
    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end2 - start2).count();

    cout << "- Without Multihread - " << endl;
    cout << setprecision(15) << "Reference PI : " << M_PI << endl; // print reference value of pi
    cout << setprecision(15) << "Calculated PI: " << sum2 << endl; // print calculated value of pi
    cout << "Duration: " << duration2 << " ms" << endl;
    cout << "============================================" << endl;

    return 0;
}