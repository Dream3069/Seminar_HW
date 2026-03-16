#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

struct Account {
    double balance;
    std::mutex mtx;

    Account(double b) : balance(b) {}
};

std::vector<Account*> accounts;
double initial_total = 0;
std::mutex cout_mutex;

void transfer(int from, int to, double amount) {
    if (from == to || amount <= 0) return;
    if (from < 0 || from >= accounts.size() || to < 0 || to >= accounts.size()) return;

    auto id = std::this_thread::get_id();

    if (from < to) {
        std::lock_guard<std::mutex> lock1(accounts[from]->mtx);
        std::lock_guard<std::mutex> lock2(accounts[to]->mtx);

        if (accounts[from]->balance >= amount) {
            accounts[from]->balance -= amount;
            accounts[to]->balance += amount;

            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Thread " << id << " transferred " << amount
                << " from " << from << " to " << to << std::endl;
        }
    }
    else {
        std::lock_guard<std::mutex> lock1(accounts[to]->mtx);
        std::lock_guard<std::mutex> lock2(accounts[from]->mtx);

        if (accounts[from]->balance >= amount) {
            accounts[from]->balance -= amount;
            accounts[to]->balance += amount;

            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Thread " << id << " transferred " << amount
                << " from " << from << " to " << to << std::endl;
        }
    }
}

void client_work(int id) {
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Client " << id << " started" << std::endl;
    }

    for (int i = 0; i < 5; i++) {
        int from = rand() % accounts.size();
        int to = rand() % accounts.size();
        double amount = (rand() % 30) + 10;

        transfer(from, to, amount);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Client " << id << " finished" << std::endl;
    }
}

void n2() {
    std::vector<double> balances = { 1000, 1500, 2000, 2500 };

    std::cout << "Initial balances: ";
    for (double b : balances) {
        std::cout << b << " ";
        accounts.push_back(new Account(b));
        initial_total += b;
    }
    std::cout << std::endl << std::endl;

    std::vector<std::thread> threads;
    int num_clients = 3;

    for (int i = 0; i < num_clients; i++) {
        threads.push_back(std::thread(client_work, i));
    }

    for (auto& t : threads) {
        t.join();
    }

    double final_total = 0;
    std::cout << "\nFinal balances: ";
    for (size_t i = 0; i < accounts.size(); i++) {
        double bal = accounts[i]->balance;
        final_total += bal;
        std::cout << bal << " ";
    }
    std::cout << std::endl;

    std::cout << "Initial total: " << initial_total << std::endl;
    std::cout << "Final total: " << final_total << std::endl;

    for (auto acc : accounts) {
        delete acc;
    }
    accounts.clear();
}