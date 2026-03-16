#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

static class TaskTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;

public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }

    long long getDuration() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count();
    }

    void measureSorting(std::vector<int>& data) {
        start();
        std::sort(data.begin(), data.end());
        stop();
        std::cout << "Sorting time: " << getDuration() << " milliseconds" << std::endl;
    }
};

static std::vector<int> generateRandomArray(size_t size) {
    std::vector<int> arr(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1000000);

    for (auto& num : arr) {
        num = dist(gen);
    }

    return arr;
}

void n4() {
    const size_t ARRAY_SIZE = 100000;
    TaskTimer timer;

    // Первый массив случайных чисел
    std::vector<int> data1 = generateRandomArray(ARRAY_SIZE);
    timer.measureSorting(data1);

    // Второй массив случайных чисел
    std::vector<int> data2 = generateRandomArray(ARRAY_SIZE);
    timer.measureSorting(data2);

}