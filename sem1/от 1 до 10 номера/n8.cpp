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
};


void n8()
{
    std::vector<Person*> people;
    Student* s = new Student("Andrey Shestakov", "322");

    s->add_grade(4.5);
    s->add_grade(4.2);
    s->add_grade(3.8);

    s->print();
}
