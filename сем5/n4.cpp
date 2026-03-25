#include <iostream>
#include <queue>
#include <mutex>
#include <semaphore>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <string>
#include <sstream>

static std::mutex console_mutex;

struct PrintJob {
    std::string doc;
    int priority;
    std::thread::id tid;
    bool completed;

    bool operator<(const PrintJob& other) const {
        return priority < other.priority;
    }
};

class PrinterQueue {
private:
    int n_printers;
    std::counting_semaphore<> semaphore;
    std::mutex mtx;
    std::priority_queue<PrintJob> queue;
    std::atomic<int> interrupted_jobs;

    void safe_print(const std::string& message) {
        std::lock_guard<std::mutex> lock(console_mutex);
        std::cout << message;
    }

public:
    PrinterQueue(int printers)
        : n_printers(printers), semaphore(printers), interrupted_jobs(0) {
    }

    void printJob(std::string doc, int priority, int timeout_ms) {
        std::thread::id tid = std::this_thread::get_id();

        std::stringstream ss;
        ss << tid;
        std::string tid_str = ss.str();

        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push({ doc, priority, tid, false });
            safe_print("  Thread " + tid_str + " (priority " + std::to_string(priority) +
                ") added job: " + doc + "\n");
        }

        if (!semaphore.try_acquire_for(std::chrono::milliseconds(timeout_ms))) {
            std::lock_guard<std::mutex> lock(mtx);
            std::priority_queue<PrintJob> temp;
            bool found = false;
            while (!queue.empty()) {
                auto job = queue.top();
                queue.pop();
                if (job.tid == tid && job.doc == doc) {
                    interrupted_jobs++;
                    safe_print("  Thread " + tid_str + " (priority " + std::to_string(priority) +
                        ") job INTERRUPTED (timeout)\n");
                    found = true;
                    continue;
                }
                temp.push(job);
            }
            queue = temp;
            if (!found) {
                semaphore.release();
            }
            return;
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            std::priority_queue<PrintJob> temp;
            while (!queue.empty()) {
                auto job = queue.top();
                queue.pop();
                if (!(job.tid == tid && job.doc == doc)) {
                    temp.push(job);
                }
            }
            queue = temp;
        }

        safe_print("  Thread " + tid_str + " (priority " + std::to_string(priority) +
            ") PRINTING: " + doc + "\n");

        int print_time = 100;
        if (priority > 15) {
            print_time = 80;
        }
        else if (priority < 5) {
            print_time = 120;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(print_time));
        std::this_thread::yield();

        safe_print("  Thread " + tid_str + " (priority " + std::to_string(priority) +
            ") FINISHED printing\n");
        semaphore.release();
    }

    int get_interrupted_count() const { return interrupted_jobs.load(); }
};

void printer_worker(PrinterQueue& pq, std::string doc, int priority, int id) {
    pq.printJob(doc, priority, 50);
}

void n4() {
    std::cout << "Printer Queue with priorities and interruptible jobs\n";
    PrinterQueue pq(2);
    std::vector<std::thread> threads;

    threads.emplace_back(printer_worker, std::ref(pq), "Low priority doc", 1, 1);
    threads.emplace_back(printer_worker, std::ref(pq), "High priority doc", 10, 2);
    threads.emplace_back(printer_worker, std::ref(pq), "Medium priority doc", 5, 3);
    threads.emplace_back(printer_worker, std::ref(pq), "Urgent doc", 20, 4);

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "  Interrupted jobs: " << pq.get_interrupted_count() << "\n";
}