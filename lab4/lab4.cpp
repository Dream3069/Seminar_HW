#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <sycl/sycl.hpp>

using namespace sycl;

void vectorAddCPU(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C) {
    for (int i = 0; i < A.size(); ++i) {
        C[i] = A[i] + B[i];
    }
}

int main() {
    try {
        queue q(default_selector_v);

        std::cout << "z1" << std::endl;
        std::cout << "Device: " << q.get_device().get_info<info::device::name>() << std::endl;

        const int N = 1000000;
        std::vector<float> A(N), B(N), C_cpu(N), C_gpu(N);

        for (int i = 0; i < N; ++i) {
            A[i] = static_cast<float>(i);
            B[i] = static_cast<float>(2 * i);
        }

        std::cout << "\nCPU computation" << std::endl;
        auto start_cpu = std::chrono::high_resolution_clock::now();
        vectorAddCPU(A, B, C_cpu);
        auto end_cpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_cpu = end_cpu - start_cpu;
        std::cout << "CPU time: " << duration_cpu.count() << " ms" << std::endl;

        std::cout << "\nSYCL computation" << std::endl;
        auto start_gpu = std::chrono::high_resolution_clock::now();
        {
            buffer bufA(A.data(), range<1>(N));
            buffer bufB(B.data(), range<1>(N));
            buffer bufC(C_gpu.data(), range<1>(N));

            q.submit([&](handler& h) {
                auto accA = bufA.get_access<access::mode::read>(h);
                auto accB = bufB.get_access<access::mode::read>(h);
                auto accC = bufC.get_access<access::mode::write>(h);

                h.parallel_for(range<1>(N), [=](id<1> i) {
                    accC[i] = accA[i] + accB[i];
                    });
                });
            q.wait();
        }
        auto end_gpu = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_gpu = end_gpu - start_gpu;
        std::cout << "SYCL time: " << duration_gpu.count() << " ms" << std::endl;

        std::cout << "\nVerification" << std::endl;
        bool success = true;
        for (int i = 0; i < N; ++i) {
            if (std::abs(C_cpu[i] - C_gpu[i]) > 1e-5) {
                success = false;
                break;
            }
        }

        std::cout << "Result: " << (success ? "SUCCESS" : "FAIL") << std::endl;
        std::cout << "Speedup: " << duration_cpu.count() / duration_gpu.count() << "x" << std::endl;

    }
    catch (const sycl::exception& e) {
        std::cerr << "SYCL error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}