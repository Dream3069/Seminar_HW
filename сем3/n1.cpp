#include <iostream>
#include <chrono>

long long sumUpToN(long long N) {
    long long sum = 0;
    for (long long i = 1; i <= N; ++i) {
        sum += i;
    }
    return sum;
}

void n1() {
    const long long N = 100000000; 

    auto start = std::chrono::high_resolution_clock::now();

    long long result = sumUpToN(N);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "The sum of the numbers from 1 to " << N << " = " << result << std::endl;
    std::cout << "Time work: " << duration.count() << " milliseconds" << std::endl;

}