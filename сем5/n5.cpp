#include <iostream>
#include <queue>
#include <mutex>
#include <semaphore>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <sstream>

static std::mutex console_mutex;

struct Task {
    int id;
    int required_slots;
    int duration_ms;
    int priority;

    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

class TaskScheduler {
private:
    std::priority_queue<Task> task_queue;
    std::counting_semaphore<> resource_semaphore;
    std::mutex queue_mutex;
    std::atomic<int> completed_tasks;
    std::condition_variable cv;
    std::atomic<bool> stop_flag;
    int total_slots;

    void safe_print(const std::string& message) {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << message;
    }

public:
    TaskScheduler(int total_slots)
        : resource_semaphore(total_slots), completed_tasks(0), stop_flag(false), total_slots(total_slots) {
    }

    void submit(Task task) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        task_queue.push(task);
        cv.notify_one();

        std::stringstream ss;
        ss << "  Task " << task.id << " submitted (priority " << task.priority
            << ", slots: " << task.required_slots << ")\n";
        safe_print(ss.str());
    }

    inline void execute_task(Task& task) {
        std::this_thread::sleep_for(std::chrono::milliseconds(task.duration_ms));
    }

    void worker() {
        while (!stop_flag) {
            Task task;
            bool has_task = false;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait(lock, [this] { return !task_queue.empty() || stop_flag; });
                if (stop_flag && task_queue.empty()) break;
                if (!task_queue.empty()) {
                    task = task_queue.top();
                    task_queue.pop();
                    has_task = true;
                }
            }

            if (!has_task) continue;

            std::thread::id tid = std::this_thread::get_id();
            std::stringstream ss_tid;
            ss_tid << tid;
            std::string tid_str = ss_tid.str();

            auto start_wait = std::chrono::steady_clock::now();

            bool acquired = true;
            for (int i = 0; i < task.required_slots; i++) {
                if (!resource_semaphore.try_acquire_for(std::chrono::milliseconds(100))) {
                    acquired = false;
                    for (int j = 0; j < i; j++) {
                        resource_semaphore.release();
                    }
                    break;
                }
            }

            if (!acquired) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                task_queue.push(task);
                cv.notify_one();
                std::stringstream ss;
                ss << "  Thread " << tid_str << " FAILED to acquire resources for Task "
                    << task.id << " (timeout), re-queuing\n";
                safe_print(ss.str());
                continue;
            }

            auto wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_wait).count();

            std::stringstream ss;
            ss << "  Thread " << tid_str << " executing Task " << task.id
                << " (priority " << task.priority
                << ", slots: " << task.required_slots
                << ", waited: " << wait_time << "ms)\n";
            safe_print(ss.str());

            execute_task(task);

            for (int i = 0; i < task.required_slots; i++) {
                resource_semaphore.release();
            }

            completed_tasks++;
            std::stringstream ss2;
            ss2 << "  Thread " << tid_str << " completed Task " << task.id << "\n";
            safe_print(ss2.str());

            std::this_thread::yield();
        }
    }

    void stop() {
        stop_flag = true;
        cv.notify_all();
    }

    int get_completed_tasks() const { return completed_tasks.load(); }
};

void n5() {
    std::cout << "Task Scheduler with resource limits\n";
    TaskScheduler scheduler(4);
    std::vector<std::thread> workers;

    for (int i = 0; i < 3; i++) {
        workers.emplace_back(&TaskScheduler::worker, &scheduler);
    }

    scheduler.submit({ 1, 2, 100, 1 });
    scheduler.submit({ 2, 1, 50, 5 });
    scheduler.submit({ 3, 3, 80, 3 });
    scheduler.submit({ 4, 2, 60, 10 });
    scheduler.submit({ 5, 1, 40, 7 });

    std::this_thread::sleep_for(std::chrono::seconds(3));

    scheduler.stop();

    for (auto& t : workers) {
        t.join();
    }

    std::cout << "  Completed tasks: " << scheduler.get_completed_tasks() << "\n";
}