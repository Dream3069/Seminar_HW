#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

template<typename Key, typename Value>
class Cache {
private:
    std::map<Key, Value> data;
    std::mutex mtx;
    std::condition_variable cv;

    inline bool has_key(const Key& k) {
        return data.find(k) != data.end();
    }

public:
    void set(const Key& key, const Value& value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            data[key] = value;
            std::cout << "Thread " << std::this_thread::get_id()
                << " SET: " << key << " = " << value << std::endl;
            cv.notify_all();
        }
        std::this_thread::yield();
    }

    Value get(const Key& key) {
        std::unique_lock<std::mutex> lock(mtx);
        auto id = std::this_thread::get_id();

        std::cout << "Thread " << id << " waiting for key: " << key << std::endl;

        while (!has_key(key)) {
            cv.wait(lock);
            std::cout << "Thread " << id << " woke up, checking for " << key << std::endl;
        }

        Value val = data[key];
        std::cout << "Thread " << id << " GOT: " << key << " = " << val << std::endl;

        std::this_thread::yield();
        return val;
    }

    void print_all() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "\nCache Contents: " << std::endl;
        for (const auto& p : data) {
            std::cout << p.first << " -> " << p.second << std::endl;
        }
    }

    void run_test() {
        std::cout << "\nStarting Cache test with writers and readers..." << std::endl;

        std::vector<std::thread> threads;

        for (int i = 0; i < 2; ++i) {
            threads.push_back(std::thread([this, i]() {
                for (int j = 0; j < 3; ++j) {
                    std::string key = "key" + std::to_string(i * 3 + j);
                    int value = rand() % 100;
                    set(key, value);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                }));
        }

        for (int i = 0; i < 3; ++i) {
            threads.push_back(std::thread([this]() {
                for (int j = 0; j < 2; ++j) {
                    std::string key = "key" + std::to_string(rand() % 6);
                    get(key);
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
                }));
        }

        for (auto& t : threads) {
            t.join();
        }

        print_all();
    }
};

void n5() {
    Cache<std::string, int> cache;
    cache.run_test();
}

#endif