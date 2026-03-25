#include <iostream>
#include <vector>
#include <mutex>
#include <semaphore>
#include <thread>
#include <chrono>
#include <random>
#include <memory>

// Глобальный мьютекс для синхронизации вывода в консоль
static std::mutex console_mutex;

template<typename T>
class SemaphoreBuffer {
private:
    std::vector<std::vector<T>> buffers;
    std::vector<std::unique_ptr<std::counting_semaphore<>>> empty;
    std::vector<std::unique_ptr<std::counting_semaphore<>>> full;
    std::vector<std::mutex> mtx;
    std::atomic<int> timeout_count;

public:
    SemaphoreBuffer(int num_buffers, int size)
        : buffers(num_buffers),
        mtx(num_buffers),
        timeout_count(0) {

        for (int i = 0; i < num_buffers; i++) {
            empty.push_back(std::make_unique<std::counting_semaphore<>>(size));
            full.push_back(std::make_unique<std::counting_semaphore<>>(0));
            buffers[i].reserve(size);
        }
    }

    void safe_print(const std::string& message) {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << message;
    }

    void produce(T value, int buffer_index, int timeout_ms) {
        std::thread::id tid = std::this_thread::get_id();

        if (!empty[buffer_index]->try_acquire_for(std::chrono::milliseconds(timeout_ms))) {
            timeout_count++;
            safe_print("  Thread " + std::to_string(std::hash<std::thread::id>{}(tid)) +
                " TIMEOUT on produce to buffer " + std::to_string(buffer_index) + "\n");
            throw std::runtime_error("Produce timeout");
        }

        {
            std::lock_guard<std::mutex> lock(mtx[buffer_index]);
            buffers[buffer_index].push_back(value);
            safe_print("  Thread " + std::to_string(std::hash<std::thread::id>{}(tid)) +
                " PRODUCED " + std::to_string(value) + " to buffer " +
                std::to_string(buffer_index) + "\n");
        }

        full[buffer_index]->release();
    }

    T consume(int buffer_index, int timeout_ms) {
        std::thread::id tid = std::this_thread::get_id();

        if (!full[buffer_index]->try_acquire_for(std::chrono::milliseconds(timeout_ms))) {
            timeout_count++;
            safe_print("  Thread " + std::to_string(std::hash<std::thread::id>{}(tid)) +
                " TIMEOUT on consume from buffer " + std::to_string(buffer_index) + "\n");
            throw std::runtime_error("Consume timeout");
        }

        T value;
        {
            std::lock_guard<std::mutex> lock(mtx[buffer_index]);
            value = buffers[buffer_index].back();
            buffers[buffer_index].pop_back();
            safe_print("  Thread " + std::to_string(std::hash<std::thread::id>{}(tid)) +
                " CONSUMED " + std::to_string(value) + " from buffer " +
                std::to_string(buffer_index) + "\n");
        }

        empty[buffer_index]->release();
        return value;
    }

    int get_timeout_count() const { return timeout_count.load(); }

    int get_random_buffer() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, buffers.size() - 1);
        return dis(gen);
    }
};

void producer(SemaphoreBuffer<int>& buffer, int id) {
    for (int i = 0; i < 5; i++) {
        try {
            int buf_idx = buffer.get_random_buffer();
            buffer.produce(i + id * 100, buf_idx, 50);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::this_thread::yield();
        }
        catch (...) {
            i--;
        }
    }
}

void consumer(SemaphoreBuffer<int>& buffer, int id) {
    for (int i = 0; i < 5; i++) {
        try {
            int buf_idx = buffer.get_random_buffer();
            buffer.consume(buf_idx, 50);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::this_thread::yield();
        }
        catch (...) {
            i--;
        }
    }
}

void n3() {
    SemaphoreBuffer<int> buffer(3, 2);
    std::vector<std::thread> producers, consumers;

    for (int i = 0; i < 3; i++) {
        producers.emplace_back(producer, std::ref(buffer), i);
    }
    for (int i = 0; i < 3; i++) {
        consumers.emplace_back(consumer, std::ref(buffer), i);
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    std::cout << "  Timeouts: " << buffer.get_timeout_count() << "\n";
}