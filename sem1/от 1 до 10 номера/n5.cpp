// дз1 по пирв.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <algorithm> // для sort, remove_if
#include <utility> // для pair
#include <fstream>  // для работы с файлами
#include <cstring>  // для memcpy

#pragma pack(push, 1)
struct FileHeader {
    char signature[4];
    int version;
    int studentCount;
    int subjectCount;
};
#pragma pack(pop)

static void saveToFile(const std::vector<std::vector<double>>& grades, const std::string& filename) {
    // Открываем файл для бинарной записи
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error open file for recording!" << std::endl;
        return;
    }

    // Подготавливаем заголовок
    FileHeader header;
    memcpy(header.signature, "GRD8", 4);  // копируем сигнатуру
    header.version = 1;
    header.studentCount = grades.size();
    header.subjectCount = grades[0].size();

    // Выводим размер структуры
    std::cout << "Size FileHeader: " << sizeof(FileHeader) << " bytes" << std::endl;

    // Записываем заголовок
    file.write(reinterpret_cast<char*>(&header), sizeof(FileHeader));

    // Записываем все оценки
    for (const auto& student : grades) {
        for (double grade : student) {
            file.write(reinterpret_cast<char*>(&grade), sizeof(double));
        }
    }

    file.close();
    std::cout << "Data save in file: " << filename << std::endl;
}

std::vector<std::vector<double>> loadFromFile(const std::string& filename) {
    std::vector<std::vector<double>> grades;

    // Открываем файл для бинарного чтения
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error open file for reading!" << std::endl;
        return grades;
    }

    // Читаем заголовок
    FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(FileHeader));

    // Проверяем сигнатуру
    if (memcmp(header.signature, "GRD8", 4) != 0) {
        std::cout << "Error: wrong signature file!" << std::endl;
        return grades;
    }

    std::cout << "Version file: " << header.version << std::endl;

    // Читаем оценки
    grades.resize(header.studentCount, std::vector<double>(header.subjectCount));

    for (int i = 0; i < header.studentCount; i++) {
        for (int j = 0; j < header.subjectCount; j++) {
            file.read(reinterpret_cast<char*>(&grades[i][j]), sizeof(double));
        }
    }

    file.close();
    std::cout << "Uploaded students: " << header.studentCount
        << ", subjects: " << header.subjectCount << std::endl;

    return grades;
}

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
        students.push_back({ i, studentAverages[i] });
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

static int filter_students(std::vector<double> studentAverages, double min_grades, int N) {

    studentAverages.erase(
        std::remove_if(studentAverages.begin(), studentAverages.end(), [min_grades](double gr) {
            return gr > min_grades;
            }
        ), studentAverages.end());

    return (int)studentAverages.size();
}

static void count_students(std::vector<double>& studentAverages, int N) {
    int good_students = 0;
    int bad_students = 0;

    for (int i = 0; i < N; i++) {
        if (studentAverages[i] >= 4.5) { good_students++; }
        if (studentAverages[i] < 3) { bad_students++; }
    }
    std::cout << "Good student: " << good_students << std::endl;
    std::cout << "Bad student: " << bad_students << std::endl;
}
class Students {


};

void n5()
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


    std::cout << "Place\tIndex\t\tAverage" << std::endl;
    for (size_t i = 0; i < N; i++) {
        std::cout << i + 1 << "\tstudent " << students_sorted[i].first + 1
            << "\t" << students_sorted[i].second << std::endl;
    }

    std::cout << "Write min_grades" << std::endl;
    double min_grades;
    std::cin >> min_grades;
    int Left_students = filter_students(studentAverages, min_grades, N);
    std::cout << "Delete student: " << N - Left_students << std::endl;
    std::cout << "Left student: " << Left_students << std::endl;
    count_students(studentAverages, N);

    saveToFile(grades, "students.dat");
    std::cout << "" << std::endl;
    auto loadedGrades = loadFromFile("students.dat");


    for (size_t i = 0; i < loadedGrades.size(); i++) {
        std::cout << "Sudent " << i + 1 << ": ";
        for (size_t j = 0; j < loadedGrades[i].size(); j++) {
            std::cout << loadedGrades[i][j] << " ";
        }
        std::cout << std::endl;
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
