#include <iostream>
#include <vector>
#include <string>
#include <queue>

class TaskVirtualThread {
private:
    std::queue<std::string> tasks;
    std::string currentTask;
    bool taskStarted;

public:
    TaskVirtualThread(const std::vector<std::string>& initialTasks) : taskStarted(false) {
        for (const auto& task : initialTasks) {
            tasks.push(task);
        }
    }

    bool hasTasks() const {
        return !tasks.empty() || taskStarted;
    }

    void runNextTask() {
        if (!hasTasks()) return;

        if (!taskStarted && !tasks.empty()) {
            currentTask = tasks.front();
            tasks.pop();
            taskStarted = true;
            std::cout << "Start task " << currentTask;
        }
        else if (taskStarted) {
            taskStarted = false;
            std::cout << "End task " << currentTask;
        }
    }
};

class TaskHyperThreadingSimulator {
private:
    TaskVirtualThread thread1;
    TaskVirtualThread thread2;

public:
    TaskHyperThreadingSimulator(const std::vector<std::string>& tasks1,
        const std::vector<std::string>& tasks2)
        : thread1(tasks1), thread2(tasks2) {
    }

    void execute() {
        while (thread1.hasTasks() || thread2.hasTasks()) {
            if (thread1.hasTasks()) {
                std::cout << "Virtual 1 ";
                thread1.runNextTask();
                std::cout << std::endl;
            }

            if (thread2.hasTasks()) {
                std::cout << "Virtual 2 ";
                thread2.runNextTask();
                std::cout << std::endl;
            }
        }
    }
};

void n7() {
    std::vector<std::string> tasks1 = { "A", "C" };
    std::vector<std::string> tasks2 = { "B", "D" };

    TaskHyperThreadingSimulator simulator(tasks1, tasks2);
    simulator.execute();

}