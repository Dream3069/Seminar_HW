// т.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class Student {
private:
    std::string name;
    std::vector<double> grades;
public:
    explicit Student(const std::string& studentname) : name(studentname) {}

    // вычисление средней оценки студента
    static double find_sred_students(const std::vector<double>& grades) {
        double sum = 0;
        for (int i = 0; i < grades.size(); i++) {
            sum += grades[i];
        }
        return sum / grades.size();
    }
    // добавление оценки
    void add_grade(double grade) {
        if (grade >= 0 && grade <= 5) {
            grades.push_back(grade);
        }
    }
    // вывод информации 
    void print_info() {
        std::cout << "students " << name << std::endl;
        std::cout << "grades: " << std::endl;
        for (int i = 0; i < grades.size(); i++) {
            std::cout << grades[i] << std::endl;
        }

        std::cout << "Average grade " << find_sred_students(grades) << std::endl;
    }
};

void n6()
{
    Student s("Gleb Bu");

    s.add_grade(4.5);
    s.add_grade(4.2);
    s.add_grade(3.8);

    s.print_info();
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
