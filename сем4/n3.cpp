#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <chrono>

template<typename T>
class Buffer {
private:
    std::vector<T> buffer;
    size_t capacity;
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;

public:
    Buffer(size_t cap) : capacity(cap) {
        buffer.reserve(capacity);
    }

    void produce(T value, int id) {
        std::unique_lock<std::mutex> lock(mtx);
        auto thread_id = std::this_thread::get_id();

        while (buffer.size() >= capacity) {
            std::cout << "Producer " << id << " [" << thread_id
                << "] waiting - buffer full (size: " << buffer.size()
                << "/" << capacity << ")" << std::endl;
            not_full.wait(lock);
            std::this_thread::yield(); 
        }

        buffer.push_back(value);
        std::cout << "Producer " << id << " [" << thread_id
            << "] produced: " << value << " (size: " << buffer.size()
            << "/" << capacity << ")" << std::endl;

        not_empty.notify_one();
        std::this_thread::yield();
    }

    T consume(int id) {
        std::unique_lock<std::mutex> lock(mtx);
        auto thread_id = std::this_thread::get_id();

        while (buffer.empty()) {
            std::cout << "Consumer " << id << " [" << thread_id
                << "] waiting - buffer empty" << std::endl;
            not_empty.wait(lock);
            std::this_thread::yield();
        }

        T value = buffer.back();
        buffer.pop_back();
        std::cout << "Consumer " << id << " [" << thread_id
            << "] consumed: " << value << " (size: " << buffer.size()
            << "/" << capacity << ")" << std::endl;

        not_full.notify_one();
        std::this_thread::yield();

        return value;
    }

    void run_test() {
        std::cout << "Buffer capacity: " << capacity << "\n" << std::endl;

        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;

        for (int i = 0; i < 2; ++i) {
            producers.push_back(std::thread([this, i]() {
                for (int j = 0; j < 4; ++j) {
                    produce(rand() % 100, i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 50));
                }
                }));
        }

        for (int i = 0; i < 2; ++i) {
            consumers.push_back(std::thread([this, i]() {
                for (int j = 0; j < 4; ++j) {
                    consume(i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 50));
                }
                }));
        }

        for (auto& t : producers) t.join();
        for (auto& t : consumers) t.join();
    }
};

void n3() {
    Buffer<int> buffer(3);
    buffer.run_test();
}
#endif