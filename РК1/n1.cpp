#include <boost/thread.hpp>
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <iomanip>

class MachineManager {
private:
    static const int MACHINE_COUNT = 3;
    static const int WORKER_COUNT = 5;

    std::vector<bool> machines_available;
    boost::mutex machines_mutex;
    boost::condition_variable machines_cv;

    std::atomic<int> active_workers;
    std::atomic<int> total_tasks_completed;

    boost::mutex output_mutex;

    void worker_task(int worker_id) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> work_time_dist(1, 3);

        while (true) {
            int machine_id = -1;

            {
                boost::unique_lock<boost::mutex> lock(machines_mutex);

                machines_cv.wait(lock, [this]() {
                    for (int i = 0; i < MACHINE_COUNT; ++i) {
                        if (machines_available[i]) return true;
                    }
                    return false;
                    });

                for (int i = 0; i < MACHINE_COUNT; ++i) {
                    if (machines_available[i]) {
                        machines_available[i] = false;
                        machine_id = i;
                        break;
                    }
                }
            }

            if (machine_id == -1) continue;

            {
                boost::lock_guard<boost::mutex> lock(output_mutex);
                std::cout << "Рабочий " << std::setw(2) << worker_id
                    << " начал работу на станке " << machine_id
                    << std::endl;
            }

            int work_time = work_time_gen(gen);
            boost::this_thread::sleep_for(boost::chrono::seconds(work_time));

            {
                boost::lock_guard<boost::mutex> lock(machines_mutex);
                machines_available[machine_id] = true;
                total_tasks_completed++;

                {
                    boost::lock_guard<boost::mutex> output_lock(output_mutex);
                    std::cout << "Рабочий " << std::setw(2) << worker_id
                        << " завершил работу на станке " << machine_id
                        << " (работал " << work_time << " сек)"
                        << std::endl;
                }
            }

            machines_cv.notify_one();

            boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
        }
    }

    std::uniform_int_distribution<> work_time_gen{ 1, 3 };

public:
    MachineManager() : machines_available(MACHINE_COUNT, true),
        active_workers(WORKER_COUNT),
        total_tasks_completed(0) {}

    void run_simulation(int duration_seconds) {
        std::cout << "Станков: " << MACHINE_COUNT << ", Рабочих: " << WORKER_COUNT << std::endl;
        std::cout << "Время симуляции: " << duration_seconds << " секунд\n" << std::endl;

        boost::thread_group workers;

        for (int i = 0; i < WORKER_COUNT; ++i) {
            workers.create_thread(boost::bind(&MachineManager::worker_task, this, i));
        }

        boost::this_thread::sleep_for(boost::chrono::seconds(duration_seconds));

        workers.interrupt_all();
        workers.join_all();

        std::cout << "\nВсего выполнено задач: " << total_tasks_completed << std::endl;
    }
};

void n1() {
    MachineManager manager;
    manager.run_simulation(10);
}