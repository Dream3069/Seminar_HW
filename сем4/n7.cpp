#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

template<typename T>
class PriorityQueue {
private:
    struct Item {
        T value;
        int priority;

        bool operator<(const Item& other) const {
            return priority < other.priority;
        }
    };

    std::priority_queue<Item> queue;
    std::mutex mtx;
    std::condition_variable cv;
    std::mutex cout_mtx;

public:
    void push(T value, int priority) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto id = std::this_thread::get_id();

            queue.push({ value, priority });

            {
                std::lock_guard<std::mutex> cout_lock(cout_mtx);
                std::cout << "Producer " << id << " pushed: " << value
                    << " (priority: " << priority << ")"
                    << " [queue size: " << queue.size() << "]" << std::endl;
            }

            cv.notify_one();
        }
        std::this_thread::yield();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        auto id = std::this_thread::get_id();

        cv.wait(lock, [this]() { return !queue.empty(); });

        Item item = queue.top();
        queue.pop();

        {
            std::lock_guard<std::mutex> cout_lock(cout_mtx);
            std::cout << "Consumer " << id << " popped: " << item.value
                << " (priority: " << item.priority << ")"
                << " [queue size: " << queue.size() << "]" << std::endl;
        }

        std::this_thread::yield();
        return item.value;
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }

    void run_test() {
        std::vector<std::thread> producers;
        std::vector<std::thread> consumers;

        consumers.push_back(std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            for (int i = 0; i < 6; ++i) {
                pop();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            }));

        producers.push_back(std::thread([this]() {
            push("Low Priority 1", 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            push("Low Priority 2", 1);
            }));

        producers.push_back(std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            push("Medium Priority 1", 5);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            push("Medium Priority 2", 5);
            }));

        producers.push_back(std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            push("High Priority 1", 9);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            push("High Priority 2", 9);
            }));

        for (auto& t : producers) {
            t.join();
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        for (auto& t : consumers) {
            t.detach();
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\nFinal queue size: " << size() << std::endl;
    }
};

void n7() {
    PriorityQueue<std::string> pq;
    pq.run_test();
}
#endif