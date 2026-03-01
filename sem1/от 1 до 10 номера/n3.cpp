// дз1 по пирв.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <algorithm> // для sort
#include <utility> // для pair


static std::vector<double> find_sred_students(const std::vector<std::vector<double>>& grades, const int N, const int M) {
    std::vector<double> result;
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < M; j++) {
            sum += grades[i][j];
        }
        result.push_back(sum / M);
        //std::cout << "Average grade of student" << i+1 << ": " << sum / M << std::endl;
    }
    return result;
}


static std::vector<double> find_sred_subject(const std::vector<std::vector<double>>& grades, const int N, const int M) {
    std::vector<double> result;
    for (int j = 0; j < M; j++) {
        double sum = 0;
        for (int i = 0; i < N; i++) {
            sum += grades[i][j];
        }
        result.push_back(sum / N);
        //std::cout << "Average grade of subject" << j+1 << ": " << sum / N << std::endl;
    }
    return result;
}

static double find_max_sred(const std::vector<std::vector<double>>& grades, const int N, const int M) {
    double max = -1;
    int index = 0;
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < M; j++) {
            sum += grades[i][j];
        }

        if (max < sum / N) {
            max = sum / N;
            index = i;
        }
    }
    return index;
}

static std::vector<std::pair<int, double>> sort_students(const std::vector<double>& studentAverages, const int N, const int M) {
    std::vector<std::pair<int, double>> students;

    for (int i = 0; i < N; i++) {
        students.push_back({i, studentAverages[i]});
    }
    std::sort(students.begin(), students.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
        }
    );


    
    return students;
}



void n3()
{
    std::cout << "Write count of students\n";
    int N;
    std::cin >> N;
    std::cout << "Write count of subject\n";
    int M;
    std::cin >> M;
    if (N <= 0) {
        std::cout << "Count of students will be < 0";
        return;
    }

    double gr;
    std::vector<std::vector<double>> grades(N, std::vector<double>(M));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            std::cout << "Write grade of student " << i + 1 << " for subject " << j + 1 << std::endl;
            std::cin >> gr;
            if ((gr >= 0) && (gr <= 5)) {
                grades[i][j] = gr;
            }
            else {
                std::cout << "Error! grade will be < 0 and > 5" << std::endl;
                break;
            }
        }
    }

    std::vector<double> studentAverages = find_sred_students(grades, N, M);
    std::vector<double> subjectAverages = find_sred_subject(grades, N, M);
    int bestStudent = find_max_sred(grades, N, M);
    std::vector<std::pair<int, double>> students_sorted = sort_students(studentAverages, N, M);


    // Вывод результатов
    std::cout << "\n--- result ---" << std::endl;

    std::cout << "\nAverage of students:" << std::endl;
    for (int i = 0; i < N; i++) {
        std::cout << "student " << i + 1 << ": " << studentAverages[i] << std::endl;
    }

    std::cout << "\nAverage of sibject:" << std::endl;
    for (int j = 0; j < M; j++) {
        std::cout << "sibject " << j + 1 << ": " << subjectAverages[j] << std::endl;
    }

    if (bestStudent != -1) {
        std::cout << "\nMax average student: student " << bestStudent + 1
            << " (Average: " << studentAverages[bestStudent] << ")" << std::endl;
    }

    // Вывод результатов
    std::cout << "Place\tIndex\t\tAverage" << std::endl;
    for (size_t i = 0; i < N; i++) {
        std::cout << i + 1 << "\tstudent " << students_sorted[i].first + 1
            << "\t" << students_sorted[i].second << std::endl;
    }
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
