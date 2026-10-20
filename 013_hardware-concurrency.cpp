// 013_hardware-concurrency.cpp
// demonstrates the use of std::thread::hardware_concurrency to determine the number of concurrent threads supported by the implementation.
#include <iostream>
#include <thread>

int main()
{
    // Get the number of concurrent threads supported by the implementation
    std::cout << std::thread::hardware_concurrency() << " concurrent threads supported."<< std::endl;
    return 0;
}