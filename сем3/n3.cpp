#include <iostream>
#include <chrono>

void n3() {
    long long total_seconds;

    std::cout << "Enter the number of seconds : ";
    std::cin >> total_seconds;

    std::chrono::seconds total_duration(total_seconds);

    auto hours = std::chrono::duration_cast<std::chrono::hours>(total_duration);
    auto remaining_after_hours = total_duration - hours;

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(remaining_after_hours);
    auto seconds = remaining_after_hours - minutes;

    std::cout << hours.count() << " hour ";
    std::cout << minutes.count() << " minute ";
    std::cout << seconds.count() << " second" << std::endl;

}