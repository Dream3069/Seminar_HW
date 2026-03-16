#include <iostream>
#include <chrono>
#include <thread>

void n2() {
    int N;

    std::cout << "seconds to count down: ";
    std::cin >> N;

    while (N > 0) {
        std::cout << "still time left: " << N << " s" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        --N;
    }

    std::cout << "Time out!" << std::endl;

}