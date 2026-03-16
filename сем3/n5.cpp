#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

// Пузырьковая сортировка
void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Сортировка вставками
void insertionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// Сортировка слиянием
void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSortHelper(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, left, mid);
        mergeSortHelper(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void mergeSort(std::vector<int>& arr) {
    if (!arr.empty()) mergeSortHelper(arr, 0, arr.size() - 1);
}

// Быстрая сортировка
int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortHelper(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortHelper(arr, low, pi - 1);
        quickSortHelper(arr, pi + 1, high);
    }
}

void quickSort(std::vector<int>& arr) {
    if (!arr.empty()) quickSortHelper(arr, 0, arr.size() - 1);
}

// Стандартная сортировка
void stdSort(std::vector<int>& arr) {
    std::sort(arr.begin(), arr.end());
}

// Функция для измерения времени
template<typename Func>
long long measureTime(std::vector<int> arr, Func sortFunc) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFunc(arr);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void n5() {
    std::vector<int> arr = {
    64, 34, 25, 12, 22, 11, 90, 88, 45, 33,
    77, 19, 5, 29, 41, 73, 56, 8, 37, 82,
    15, 67, 39, 94, 18, 3, 71, 84, 50, 27,
    61, 43, 98, 31, 59, 20, 86, 13, 47, 53,
    79, 24, 69, 36, 92, 7, 55, 42, 95, 16,
    64, 34, 25, 12, 22, 11, 90, 88, 45, 33,
    77, 19, 5, 29, 41, 73, 56, 8, 37, 82,
    15, 67, 39, 94, 18, 3, 71, 84, 50, 27,
    61, 43, 98, 31, 59, 20, 86, 13, 47, 53,
    79, 24, 69, 36, 92, 7, 55, 42, 95, 16,
    64, 34, 25, 12, 22, 11, 90, 88, 45, 33,
    77, 19, 5, 29, 41, 73, 56, 8, 37, 82,
    15, 67, 39, 94, 18, 3, 71, 84, 50, 27,
    61, 43, 98, 31, 59, 20, 86, 13, 47, 53,
    79, 24, 69, 36, 92, 7, 55, 42, 95, 16,
    64, 34, 25, 12, 22, 11, 90, 88, 45, 33,
    77, 19, 5, 29, 41, 73, 56, 8, 37, 82,
    15, 67, 39, 94, 18, 3, 71, 84, 50, 27,
    61, 43, 98, 31, 59, 20, 86, 13, 47, 53,
    79, 24, 69, 36, 92, 7, 55, 42, 95, 16,
    64, 34, 25, 12, 22, 11, 90, 88, 45, 33,
    77, 19, 5, 29, 41, 73, 56, 8, 37, 82,
    15, 67, 39, 94, 18, 3, 71, 84, 50, 27,
    61, 43, 98, 31, 59, 20, 86, 13, 47, 53,
    79, 24, 69, 36, 92, 7, 55, 42, 95, 16,
    64, 34, 25, 12, 22, 11, 90, 88, 45, 33,
    77, 19, 5, 29, 41, 73, 56, 8, 37, 82,
    15, 67, 39, 94, 18, 3, 71, 84, 50, 27,
    61, 43, 98, 31, 59, 20, 86, 13, 47, 53,
    79, 24, 69, 36, 92, 7, 55, 42, 95, 16,
    };

    std::cout << "Bubble Sort: " << measureTime(arr, bubbleSort) << " ms" << std::endl;
    std::cout << "Insertion Sort: " << measureTime(arr, insertionSort) << " ms" << std::endl;
    std::cout << "Merge Sort: " << measureTime(arr, mergeSort) << " ms" << std::endl;
    std::cout << "Quick Sort: " << measureTime(arr, quickSort) << " ms" << std::endl;
    std::cout << "std::sort: " << measureTime(arr, stdSort) << " ms" << std::endl;

}