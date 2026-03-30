#include <boost/thread.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <iomanip>

class StorageManager {
private:
    static const int TOTAL_WORKPIECES = 10;
    static const int MACHINE_COUNT = 3;

    boost::interprocess::interprocess_semaphore machine_semaphore;
    boost::interprocess::interprocess_semaphore workpieces_semaphore;

    std::atomic<int> processed_count;
    boost::mutex output_mutex;
    std::atomic<bool> stop_flag;

    void machine_worker(int machine_id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> work_time_dist(1, 2);

        while (!stop_flag.load()) {
            if (!workpieces_semaphore.try_wait()) {
                break;
            }

            machine_semaphore.wait();

            {
                boost::lock_guard<boost::mutex> lock(output_mutex);
                std::cout << "Станок " << machine_id
                    << " начал обработку заготовки" << std::endl;
            }

            int work_time = work_time_dist(gen);
            boost::this_thread::sleep_for(boost::chrono::seconds(work_time));

            processed_count++;

            {
                boost::lock_guard<boost::mutex> lock(output_mutex);
                std::cout << "Станок " << machine_id
                    << " завершил обработку заготовки (время: " << work_time
                    << " сек, всего обработано: " << processed_count << "/"
                    << TOTAL_WORKPIECES << ")" << std::endl;
            }

            machine_semaphore.post();
        }
    }

public:
    StorageManager() :
        machine_semaphore(MACHINE_COUNT),
        workpieces_semaphore(TOTAL_WORKPIECES),
        processed_count(0),
        stop_flag(false) {}

    void run_simulation() {
        std::cout << "Всего заготовок: " << TOTAL_WORKPIECES << std::endl;
        std::cout << "Количество станков: " << MACHINE_COUNT << "\n" << std::endl;

        auto start_time = std::chrono::high_resolution_clock::now();

        boost::thread_group machines;

        for (int i = 0; i < MACHINE_COUNT; ++i) {
            machines.create_thread(boost::bind(&StorageManager::machine_worker, this, i));
        }

        while (processed_count < TOTAL_WORKPIECES) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
        }

        stop_flag.store(true);

        machines.join_all();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "\nОбработано заготовок: " << processed_count << std::endl;
        std::cout << "Общее время обработки: " << duration.count() / 1000 << " сек" << std::endl;
    }
};

void n2() {
    StorageManager manager;
    manager.run_simulation();
}