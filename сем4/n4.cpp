#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>

template<typename T>
class Logger {
private:
    std::ofstream file;
    std::mutex file_mtx;
    std::mutex cout_mtx;

    inline std::string to_string(const T& msg) {
        std::stringstream ss;
        ss << msg;
        return ss.str();
    }

public:
    Logger(const std::string& filename) {
        file.open(filename, std::ios::app);
    }

    ~Logger() {
        if (file.is_open()) file.close();
    }

    void log(const T& message) {
        std::string str = to_string(message);
        auto id = std::this_thread::get_id();
        std::stringstream output;
        output << "Thread " << id << ": " << str;

        std::string full_message = output.str();

        {
            std::lock_guard<std::mutex> lock(file_mtx);
            file << full_message << std::endl;
            file.flush();
        }

        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << full_message << std::endl;
        }

        std::this_thread::yield();
    }

    void run_test() {
        std::cout << "\nStarting Logger test with 3 threads..." << std::endl;

        std::vector<std::thread> threads;

        for (int i = 0; i < 3; ++i) {
            threads.push_back(std::thread([this, i]() {
                auto id = std::this_thread::get_id();

                for (int j = 0; j < 3; ++j) {
                    std::string msg = "Message from thread " +
                        std::to_string(i) +
                        " #" + std::to_string(j);
                    log(msg);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10 * (i + 1)));
                }

                log("Thread " + std::to_string(i) + " finished");
                }));
        }

        for (auto& t : threads) {
            t.join();
        }

        std::cout << "Logger test completed. Check 'log.txt' file." << std::endl;
    }
};

void n4() {
    Logger<std::string> logger("log.txt");
    logger.run_test();
}

#endif