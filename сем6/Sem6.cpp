// Sem6.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <clocale>

const char* SHM_NAME = "/my_shm_buffer";
const char* SEM_NAME = "/my_sem_b";
const int NUM_CHILDREN = 4;

struct SharedData {
    int data[NUM_CHILDREN];
};

int main() {
    setlocale(LC_ALL, "Russian");

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка shm_open");
        return 1;
    }

    if (ftruncate(shm_fd, sizeof(SharedData)) == -1) {
        perror("Ошибка ftruncate");
        return 1;
    }

    SharedData* shared_data = (SharedData*)mmap(
        nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0
    );

    if (shared_data == MAP_FAILED) {
        perror("Ошибка mmap");
        return 1;
    }

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        shared_data->data[i] = 0;
    }

    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        perror("Ошибка sem_open");
        return 1;
    }

    std::cout << "Родитель: создаю " << NUM_CHILDREN << " дочерних процессов..." << std::endl;

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Ошибка fork");
            return 1;
        }

        if (pid == 0) {
            std::cout << "Дочерний процесс " << i << " (PID: " << getpid() << ") запущен." << std::endl;

            int work_time = (i + 1) * 1;
            sleep(work_time);

            shared_data->data[i] = (i + 1) * 10;
            std::cout << "Дочерний процесс " << i << " (PID: " << getpid()
                << ") записал данные: " << shared_data->data[i] << std::endl;

            std::cout << "Дочерний процесс " << i << " (PID: " << getpid()
                << ") достиг барьера." << std::endl;
            sem_post(sem);

            exit(EXIT_SUCCESS);
        }
    }

    std::cout << "Родитель: ожидаю завершения всех дочерних процессов..." << std::endl;

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        sem_wait(sem);
        std::cout << "Родитель: получен сигнал от дочернего процесса " << i << std::endl;
    }

    std::cout << "\nРезультат" << std::endl;
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        std::cout << "Данные от дочернего процесса " << i << ": " << shared_data->data[i] << std::endl;
    }

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        wait(nullptr);
    }

    munmap(shared_data, sizeof(SharedData));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    sem_close(sem);
    sem_unlink(SEM_NAME);

    std::cout << "\nРодитель: программа завершена, ресурсы освобождены." << std::endl;

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.