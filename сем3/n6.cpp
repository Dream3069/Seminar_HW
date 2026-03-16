#include <iostream>
#include <vector>

// Функция вычисления факториала
static long long factorial(int n) {
    long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

static class VirtualThread {
private:
    std::vector<int> numbers;
    size_t currentIndex;

public:
    VirtualThread(const std::vector<int>& nums) : numbers(nums), currentIndex(0) {}

    bool hasNext() const {
        return currentIndex < numbers.size();
    }

    void run() {
        if (hasNext()) {
            int num = numbers[currentIndex];
            ++currentIndex;
            long long result = factorial(num);
            std::cout << "calculates " << num << "! = " << result;
        }
    }

    int getCurrentNumber() const {
        if (currentIndex > 0 && currentIndex <= numbers.size()) {
            return numbers[currentIndex - 1];
        }
        return 0;
    }
};

static class HyperThreadingSimulator {
private:
    VirtualThread thread1;
    VirtualThread thread2;

public:
    HyperThreadingSimulator(const std::vector<int>& nums1, const std::vector<int>& nums2)
        : thread1(nums1), thread2(nums2) {
    }

    void execute() {
        while (thread1.hasNext() || thread2.hasNext()) {
            if (thread1.hasNext()) {
                std::cout << "Virtual 1 ";
                thread1.run();
                std::cout << std::endl;
            }

            if (thread2.hasNext()) {
                std::cout << "Virtual 2 ";
                thread2.run();
                std::cout << std::endl;
            }
        }
    }
};

void n6() {
    std::vector<int> nums1 = { 5, 10 };
    std::vector<int> nums2 = { 7, 12 };

    HyperThreadingSimulator simulator(nums1, nums2);
    simulator.execute();
}