#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <mutex>
#include <string>

using namespace std;

class BankAccount {
    int balance = 0;
    mutex mtx;
public:
    void deposit(int amount, int id) {
        lock_guard<mutex> lock(mtx);
        balance += amount;
        cout << "Клиент " + to_string(id) + " +" + to_string(amount) + " = " + to_string(balance) << endl;
    }

    bool withdraw(int amount, int id) {
        lock_guard<mutex> lock(mtx);
        if (amount > balance) return false;
        balance -= amount;
        cout << "Клиент " + to_string(id) + " -" + to_string(amount) + " = " + to_string(balance) << endl;
        return true;
    }

    int get_balance() {
        lock_guard<mutex> lock(mtx);
        return balance;
    }
};

void client(BankAccount& acc, int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 100);

    acc.deposit(dist(gen), id);
    acc.withdraw(dist(gen), id);
}

void z3() {
    BankAccount acc;
    vector<thread> threads;

    for (int i = 0; i < 10; i++) {
        threads.emplace_back(client, ref(acc), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "Финальный баланс: " + to_string(acc.get_balance()) << endl;
}