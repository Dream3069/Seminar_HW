#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class RecordingBook {
private:
    std::string bookNumber;
    std::vector<double> grades;
public:
    RecordingBook(const std::string& number) : bookNumber(number) {};
    // добавление оценки
    void add_grade(double grade) {
        if (grade >= 0 && grade <= 5) {
            grades.push_back(grade);
        }
    }

    // вычисление средней оценки студента
    double find_average() {
        double sum = 0;
        for (int i = 0; i < grades.size(); i++) {
            sum += grades[i];
        }
        return sum / grades.size();
    }

    void print() {
        std::cout << "Number book: " << bookNumber << std::endl;
        std::cout << "grades: " << std::endl;
        for (int i = 0; i < grades.size(); i++) {
            std::cout << grades[i] << " " << std::endl;
        }
        std::cout << "Average grade " << find_average() << std::endl;
    }

};

class Person {
protected:
    std::string name;
public:
    Person(const std::string& name) : name(name) {}
    virtual void print() {
        std::cout << "Person " << name << std::endl;
    }
    virtual ~Person() {}


};

class Teacher : public Person {
    std::string subject;
public:
    Teacher(const std::string& name, const std::string& subject) : Person(name), subject(subject) {}

    void print() override {
        std::cout << "Teacher " << name << " Subject " << subject << std::endl;
    }

};

class Student : public Person {
private:
    RecordingBook recordingBook;
    std::vector<double> grades;
public:
    explicit Student(const std::string& name, const std::string bookNumber) : Person(name), recordingBook(bookNumber) {}

    void add_grade(double grade) {
        recordingBook.add_grade(grade);
    }
    double find_average() {
        return recordingBook.find_average();
    }

    // вывод информации 
    virtual void print() override {
        std::cout << "students " << name << std::endl;
        recordingBook.print();
        std::cout << "Average " << find_average() << std::endl;
    }
    const std::string& getName() const { return name; }
};

class Group {
private:
    std::string group_name;
    std::vector<Student*> students;
public:
    Group(const std::string name) : group_name(name) {}

    void addStudent(Student* student) {
        if (student) { students.push_back(student); }
    }

    bool removeStudent(const std::string& studentName) {
        auto it = std::find_if(students.begin(), students.end(),
            [&studentName](Student* s) {
                return s && s->getName() == studentName;
            });

        if (it != students.end()) {
            students.erase(it);
            return true;
        }
        return false;
    }

    double calculateGroupAverage() const {
        if (students.empty()) return 0;

        double sum = 0;
        int count = 0;

        for (Student* s : students) {
            if (s) {
                sum += s->find_average();
                count++;
            }
        }

        return count > 0 ? sum / count : 0;
    }

    // Вывод всех студентов группы
    void printGroup() const {
        std::cout << "Группа: " << group_name << std::endl;
        std::cout << "Студентов: " << students.size() << std::endl;
        std::cout << "Средний балл группы: " << calculateGroupAverage() << std::endl;
        std::cout << "---" << std::endl;

        for (size_t i = 0; i < students.size(); i++) {
            if (students[i]) {
                std::cout << i + 1 << ". ";
                students[i]->print();
                std::cout << std::endl;
            }
        }
    }
};


void n9()
{
    std::vector<Person*> people;
    Student* s = new Student("Andrey Shestakov", "322");

    s->add_grade(4.5);
    s->add_grade(4.2);
    s->add_grade(3.8);

    s->print();


    // Создаем студентов (они существуют независимо)
    Student* s1 = new Student("Иван Петров", "ЗК-001");
    Student* s2 = new Student("Анна Сидорова", "ЗК-002");
    Student* s3 = new Student("Петр Иванов", "ЗК-003");

    // Добавляем оценки
    s1->add_grade(4.5);
    s1->add_grade(4.0);
    s2->add_grade(4.8);
    s2->add_grade(5.0);
    s3->add_grade(3.5);
    s3->add_grade(3.8);

    // Создаем группу и добавляем студентов
    Group group("Иу-43Б");
    group.addStudent(s1);
    group.addStudent(s2);
    group.addStudent(s3);

    // Выводим информацию о группе
    group.printGroup();

    // Удаляем студента
    group.removeStudent("Петр Иванов");
    std::cout << "\nПосле удаления:\n";
    group.printGroup();

    // Очищаем память (студенты удаляются отдельно от группы)
    delete s1;
    delete s2;
    delete s3;
}
