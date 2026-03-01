// дз11_.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include "Student.hpp"
#include "Teacher.hpp"
#include "Group.hpp"
#include "FileManager.hpp"

int main() {
    // Создание студентов
    Student* s1 = new Student("Abdrey Sh", "12345");
    Student* s2 = new Student("Dima blabla", "12346");
    Student* s3 = new Student("Gleb Bu", "12347");

    // Добавление оценок
    s1->addGrade(5);
    s1->addGrade(4);
    s1->addGrade(5);

    s2->addGrade(4);
    s2->addGrade(4);
    s2->addGrade(5);

    s3->addGrade(3);
    s3->addGrade(4);
    s3->addGrade(3);

    // Создание группы
    Group group("IU-43");
    group.addStudent(s1);
    group.addStudent(s2);
    group.addStudent(s3);

    // Вывод группы
    std::cout << "=== Current group ===\n";
    group.printAll();

    // Сортировка по имени
    std::cout << "\n=== After sort name ===\n";
    group.sortByName();
    group.printAll();

    // Сортировка по среднему баллу
    std::cout << "\n=== After sort average grade ===\n";
    group.sortByAverage();
    group.printAll();

    std::cout << "\n=== Students with average grade >= 4.0 ===\n";
    auto filtered = group.filterByAverage(4.0);
    for (const auto* s : filtered) {
        s->print();
        std::cout << "\n";
    }

    // Сохранение в файл
    std::cout << "\n=== save in fale ===\n";
    if (FileManager::saveGroup(group, "group.dat")) {
        std::cout << "Group save\n";
    }

    // Загрузка из файла
    std::cout << "\n=== uplaod for file ===\n";
    Group loadedGroup("New Group");
    if (FileManager::loadGroup(loadedGroup, "group.dat")) {
        std::cout << "Group uplaod\n";
        loadedGroup.printAll();
    }

    // Удаление студента
    std::cout << "\n=== After delete student ===\n";
    group.removeStudent("Петр Иванов");
    group.printAll();

    delete s1;
    delete s2;
    delete s3;

    for (Student* s : loadedGroup.filterByAverage(0)) {
        delete s;
    }

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
