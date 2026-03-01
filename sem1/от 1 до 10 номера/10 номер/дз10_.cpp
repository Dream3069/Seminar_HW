// дз10_.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Student.h"
#include "Teacher.h"
#include "Group.h"

int main() {
    // Создаем студентов
    Student* s1 = new Student("Andrey Shestakov", "12345");
    Student* s2 = new Student("Dmitru blabla", "12346");
    Student* s3 = new Student("Gleb Bu", "12347");

    // Добавляем оценки
    s1->addGrade(5);
    s1->addGrade(4);
    s1->addGrade(5);

    s2->addGrade(4);
    s2->addGrade(4);
    s2->addGrade(5);

    s3->addGrade(3);
    s3->addGrade(4);
    s3->addGrade(3);

    // Создаем группу
    Group group("IU-43");
    group.addStudent(s1);
    group.addStudent(s2);
    group.addStudent(s3);

    // Создаем преподавателя
    Teacher teacher("Pasha Pasha", "Goding");

    // Выводим информацию
    std::cout << "=== info of group ===\n";
    group.print();

    std::cout << "\n=== info of teacher ===\n";
    teacher.print();
    std::cout << "\n";

    // Используем inline методы
    std::cout << "Name first student: " << s1->getName() << "\n";
    std::cout << "Is there a zachetka? " << (s1->hasRecordBook() ? "Yes" : "No") << "\n";
    std::cout << "Name of group: " << group.getName() << "\n";
    std::cout << "Count student: " << group.getStudentCount() << "\n";
    std::cout << "Subject teacher: " << teacher.getSubject() << "\n";

    // Очищаем память
    delete s1;
    delete s2;
    delete s3;

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
