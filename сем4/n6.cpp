#ifndef MATRIX_PROCESSOR_H
#define MATRIX_PROCESSOR_H

#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
#include <chrono>
#include <functional>

template<typename T>
class MatrixProcessor {
private:
    std::vector<std::vector<T>> matrix;
    size_t rows;
    size_t cols;
    size_t num_threads;
    std::mutex cout_mtx;

    inline void process_rows(size_t start, size_t end, const std::function<T(T)>& func, int thread_num) {
        auto id = std::this_thread::get_id();

        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << "Thread " << id << " started processing rows "
                << start << "-" << (end - 1) << std::endl;
        }

        for (size_t r = start; r < end; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                matrix[r][c] = func(matrix[r][c]);
                std::this_thread::yield();
            }
            std::this_thread::yield();
        }

        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << "Thread " << id << " finished processing rows "
                << start << "-" << (end - 1) << std::endl;
        }
    }

public:
    MatrixProcessor(const std::vector<std::vector<T>>& mat, size_t threads)
        : matrix(mat), num_threads(threads) {
        rows = matrix.size();
        cols = (rows > 0) ? matrix[0].size() : 0;

        if (num_threads == 0) num_threads = 1;
        if (num_threads > rows) num_threads = rows;
    }

    void apply(const std::function<T(T)>& func) {
        std::vector<std::thread> threads;

        size_t rows_per_thread = rows / num_threads;
        size_t remainder = rows % num_threads;
        size_t start_row = 0;

        std::cout << "\nProcessing " << rows << "x" << cols << " matrix with "
            << num_threads << " threads" << std::endl;

        for (size_t i = 0; i < num_threads; ++i) {
            size_t current_rows = rows_per_thread + (i < remainder ? 1 : 0);
            size_t end_row = start_row + current_rows;
            
            threads.push_back(std::thread(&MatrixProcessor::process_rows, this,
                start_row, end_row, func, i));

            start_row = end_row;
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void print_matrix(const std::string& title) {
        std::lock_guard<std::mutex> lock(cout_mtx);

        std::cout << "\n" << title << ":" << std::endl;
        for (const auto& row : matrix) {
            for (T val : row) {
                std::cout << val << "\t";
            }
            std::cout << std::endl;
        }
    }
};

void n6() {
    std::vector<std::vector<int>> mat = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    MatrixProcessor<int> mp(mat, 3);

    mp.print_matrix("Original matrix");

    // возведение в квадрат
    std::cout << "\nSquare function";
    mp.apply([](int x) { return x * x; });

    mp.print_matrix("Matrix after square");

    // увеличение на 1
    std::cout << "\nIncrement function";
    mp.apply([](int x) { return x + 1; });

    mp.print_matrix("Matrix after increment");
}
#endif