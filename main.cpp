#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <future>
#include <syncstream>
#include <iomanip>

using namespace std::literals::chrono_literals;

void calculation(const std::string& name, std::chrono::seconds duration) {
    auto start_time = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(duration);

    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::osyncstream(std::cout)
        << "["<< std::fixed << std::setprecision(2) << std::setw(5) << elapsed.count() << "s] "
        << "Task" << std::setw(3) << name << " finished on thread "
        << std::this_thread::get_id() << std::endl;
}

void slow(const std::string& name) {
    calculation(name, 7s);
}

void quick(const std::string& name) {
    calculation(name, 1s);
}

void run_chain_A() {
    std::osyncstream(std::cout) << "Chain A starting on thread " << std::this_thread::get_id() << std::endl;
    quick("A1");
    slow("A2");
    quick("A3");
}

void run_chain_B() {
    std::osyncstream(std::cout) << "Chain B starting on thread " << std::this_thread::get_id() << std::endl;
    quick("B1");
    slow("B2");
    quick("B3");
}

void work() {
    std::osyncstream(std::cout) << "Main thread " << std::this_thread::get_id() << " started." << std::endl;
    auto start = std::chrono::steady_clock::now();

    std::future<void> futA = std::async(std::launch::async, run_chain_A);

    std::future<void> futB = std::async(std::launch::async, run_chain_B);

    futA.get();
    std::osyncstream(std::cout) << "Main thread: Chain A dependencies (A3) are met." << std::endl;

    futB.get();
    std::osyncstream(std::cout) << "Main thread: Chain B dependencies (B3) are met." << std::endl;

    quick("C");

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Total execution time: " << std::fixed << std::setprecision(2) << elapsed.count() << " seconds" << std::endl
              << "Work is done!" << std::endl;
}

int main() {
    work();
    return 0;
}
