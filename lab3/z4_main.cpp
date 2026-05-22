#include <iostream>
#include <vector>
#include <thread>
#include "z4_server.hpp"

int main() {
    setlocale(LC_ALL, "Russian");

    try {
        int n_threads = std::thread::hardware_concurrency();
        if (n_threads == 0) n_threads = 2;

        boost::asio::io_context io_context;
        Server s(io_context, 12345, n_threads);

        std::vector<std::thread> threads;
        for (int i = 0; i < n_threads; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
                });
        }

        std::cout << "Сервер запущен на порту 12345 с " << n_threads << " потоками." << std::endl;

        for (auto& t : threads) {
            t.join();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}