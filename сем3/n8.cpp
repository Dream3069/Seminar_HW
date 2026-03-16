#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
namespace {
    class Task {
    public:
        int value;           // число для вычисления
        int priority;        // приоритет (1-10, чем выше, тем важнее)
        int duration_ms;     // общее время выполнения
        int steps;           // количество шагов
        int currentStep;     // текущий выполненный шаг

        Task(int val, int prio, int dur, int st)
            : value(val), priority(prio), duration_ms(dur), steps(st), currentStep(0) {
        }

        bool isCompleted() const {
            return currentStep >= steps;
        }

        int getStepDuration() const {
            return duration_ms / steps;
        }

        int calculateResult() const {
            return value * value;
        }
    };

    class VirtualThread {
    private:
        std::vector<Task> tasks;

    public:
        void addTask(const Task& task) {
            tasks.push_back(task);
        }

        bool hasTasks() const {
            return !tasks.empty();
        }

        void runStep(int threadId) {
            if (tasks.empty()) return;

            auto highestPriorityTask = std::max_element(tasks.begin(), tasks.end(),
                [](const Task& a, const Task& b) {
                    return a.priority < b.priority;
                });

            Task& task = *highestPriorityTask;

            task.currentStep++;
            int stepDuration = task.getStepDuration();

            std::cout << "Virtual " << threadId
                << " steps progress " << task.currentStep << "/" << task.steps
                << " task " << task.value
                << " with prioritet " << task.priority << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(stepDuration));

            if (task.isCompleted()) {
                int result = task.calculateResult();
                std::cout << "Virtual " << threadId
                    << " end task " << task.value
                    << ": result = " << result << std::endl;

                tasks.erase(highestPriorityTask);
            }
        }
    };

    class HyperThreadingSimulator {
    private:
        VirtualThread thread1;
        VirtualThread thread2;

    public:
        HyperThreadingSimulator(VirtualThread t1, VirtualThread t2)
            : thread1(t1), thread2(t2) {
        }

        void execute() {
            while (thread1.hasTasks() || thread2.hasTasks()) {
                if (thread1.hasTasks()) {
                    thread1.runStep(1);
                }

                if (thread2.hasTasks()) {
                    thread2.runStep(2);
                }
            }
        }
    };

    int randomInt(int min, int max, std::mt19937& gen) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
}
void n8(){
    std::random_device rd;
    std::mt19937 gen(rd());

    VirtualThread thread1, thread2;

    // Генерация случайных задач для первого потока
    int numTasks1 = randomInt(2, 4, gen);
    std::cout << "generation " << numTasks1 << " task for potok 1:\n";
    for (int i = 0; i < numTasks1; ++i) {
        int value = randomInt(1, 50, gen);
        int priority = randomInt(1, 10, gen);
        int duration = randomInt(200, 1000, gen);
        int steps = randomInt(2, 5, gen);

        thread1.addTask(Task(value, priority, duration, steps));
        std::cout << "  task: value=" << value
            << ", priority=" << priority
            << ", duration=" << duration
            << ", steps=" << steps << std::endl;
    }

    // Генерация случайных задач для второго потока
    int numTasks2 = randomInt(2, 4, gen);
    std::cout << "\ngeneration " << numTasks2 << " task for potok 2:\n";
    for (int i = 0; i < numTasks2; ++i) {
        int value = randomInt(1, 50, gen);
        int priority = randomInt(1, 10, gen);
        int duration = randomInt(200, 1000, gen);
        int steps = randomInt(2, 5, gen);

        thread2.addTask(Task(value, priority, duration, steps));
        std::cout << "  task: value=" << value
            << ", priority=" << priority
            << ", duration=" << duration
            << ", steps=" << steps << std::endl;
    }

    std::cout << "\n=== Start all task ===\n\n";

    HyperThreadingSimulator simulator(thread1, thread2);
    simulator.execute();

    std::cout << "\n=== All task done ===\n";
}
