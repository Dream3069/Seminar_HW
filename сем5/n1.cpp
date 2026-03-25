#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
#include <semaphore>
#include <thread>
#include <chrono>

template<typename T>
class ResourcePool {
private:
    std::vector<T> resources;
    std::counting_semaphore<> semaphore;
    std::mutex mtx;
    std::atomic<int> failed_attempts;

public:
    ResourcePool(const std::vector<T>& res)
        : resources(res), semaphore(res.size()), failed_attempts(0) {
    }

    T acquire(int priority, int timeout_ms) {
        std::thread::id tid = std::this_thread::get_id();

        if (!semaphore.try_acquire_for(std::chrono::milliseconds(timeout_ms))) {
            failed_attempts++;
            {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "  Thread " << tid << " (priority " << priority
                    << ") FAILED to acquire resource (timeout)\n";
            }
            throw std::runtime_error("Acquire timeout");
        }

        std::lock_guard<std::mutex> lock(mtx);
        if (resources.empty()) {
            semaphore.release();
            throw std::runtime_error("No resources available");
        }

        T res = resources.back();
        resources.pop_back();

        std::cout << "  Thread " << tid << " (priority " << priority
            << ") ACQUIRED resource\n";
        return res;
    }

    void release(T res) {
        std::lock_guard<std::mutex> lock(mtx);
        resources.push_back(res);
        semaphore.release();
        std::cout << "  Thread " << std::this_thread::get_id()
            << " RELEASED resource\n";
    }

    int get_failed_attempts() const { return failed_attempts.load(); }
};

void worker(ResourcePool<int>& pool, int priority, int id) {
    try {
        std::this_thread::yield();
        int res = pool.acquire(priority, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        pool.release(res);
    }
    catch (...) {
        // timeout
    }
}

void n1() {
    std::cout << "Resource Pool with thread priorities\n";
    ResourcePool<int> pool({ 1, 2, 3 });
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back(worker, std::ref(pool), i % 3, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "  Failed attempts: " << pool.get_failed_attempts() << "\n";
}