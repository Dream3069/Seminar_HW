#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

class ParkingLot {
private:
    int capacity;
    std::counting_semaphore<> semaphore;
    std::mutex mtx;
    std::atomic<int> occupied_spaces;

public:
    ParkingLot(int cap)
        : capacity(cap), semaphore(cap), occupied_spaces(0) {
    }

    void park(bool isVIP, int timeout_ms) {
        std::thread::id tid = std::this_thread::get_id();

        if (!semaphore.try_acquire_for(std::chrono::milliseconds(timeout_ms))) {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "  Thread " << tid << " (" << (isVIP ? "VIP" : "Normal")
                << ") FAILED to park (timeout)\n";
            throw std::runtime_error("Parking timeout");
        }

        occupied_spaces++;
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "  Thread " << tid << " (" << (isVIP ? "VIP" : "Normal")
                << ") PARKED. Occupied: " << occupied_spaces.load()
                << ", Free: " << (capacity - occupied_spaces.load()) << "\n";
        }

        std::this_thread::yield();
    }

    void leave() {
        std::thread::id tid = std::this_thread::get_id();
        semaphore.release();
        occupied_spaces--;

        {
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "  Thread " << tid << " LEFT. Occupied: "
                << occupied_spaces.load()
                << ", Free: " << (capacity - occupied_spaces.load()) << "\n";
        }
    }
};

void car(ParkingLot& lot, bool isVIP, int id) {
    try {
        lot.park(isVIP, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        lot.leave();
    }
    catch (...) {}
}

void n2() {
    std::cout << "Parking Lot with VIP priority\n";
    ParkingLot lot(3);
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; i++) {
        threads.emplace_back(car, std::ref(lot), false, i);
    }
    for (int i = 0; i < 3; i++) {
        threads.emplace_back(car, std::ref(lot), true, i + 5);
    }

    for (auto& t : threads) {
        t.join();
    }
}