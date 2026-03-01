// дз1 по пирв.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
static double find_sred(double* grades, int N) {
    double sum = 0;
    for (int i = 0; i < N; i++) {
        sum += grades[i];
    }
    double result = sum / N;
    return result;
}


static double find_max(double grades[], int N) {
    double max = grades[0];
    for (int i = 1; i < N; i++) {
        if (grades[i] > max) {
            max = grades[i];
        }
    }
    return max;
}


static double find_min(double grades[], int N) {
    double min = grades[0];
    for (int i = 1; i < N; i++) {
        if (grades[i] < min) {
            min = grades[i];
        }
    }
    return min;

}


static int find_good_students(double grades[], int N, double porog) {
    int counter = 0;
    for (int i = 0; i < N; i++) {
        if (grades[i] > porog) {
            counter++;
        }
    }
    return counter;
}


void n1()
{
    std::cout << "Write count of students\n";
    int N;
    std::cin >> N;
    if (N <= 0) {
        std::cout << "Count of students will be < 0";
        return;
    }

    double gr;
    double* grades = new double[N];
    for (int i = 0; i < N; i++) {
        std::cout << "Write grade of students " << i+1 << std::endl;
        std::cin >> gr;
        if ((gr >= 0) && (gr <= 5)) {
            grades[i] = gr;
        }
    }
    std::cout << "Average:" << find_sred(grades, N) << std::endl;
    std::cout << "Max:" << find_max(grades, N) << std::endl;
    std::cout << "Min:" << find_min(grades, N) << std::endl;
    std::cout << "Write porog for function find_good_students\n";
    double porog;
    std::cin >> porog;
    std::cout << "Count of good students:" << find_good_students(grades, N, porog) << std::endl;
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
