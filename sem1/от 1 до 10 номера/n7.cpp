#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

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
    std::vector<double> grades;
public:
    explicit Student(const std::string& name) : Person(name) {}

    // вычисление средней оценки студента
    double find_sred_students() {
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
    virtual void print() override {
        std::cout << "students " << name << std::endl;
        std::cout << "grades: " << std::endl;
        for (int i = 0; i < grades.size(); i++) {
            std::cout << grades[i] << " " << std::endl;
        }

        std::cout << "Average grade " << find_sred_students() << std::endl;
    }
};




void n7()
{


    std::vector<Person*> people;
    Student* s = new Student("Andrey Shestakov");

    s->add_grade(4.5);
    s->add_grade(4.2);
    s->add_grade(3.8);

    //people.push_back(new Student("Gleb BU"));
    people.push_back(new Teacher("Pasha Panilov", "Coding"));
    people.push_back(s);
    //people.push_back(new Student("Dima Scryabikov"));
    s->print();
    std::cout << people.size() << std::endl;

    for (Person* person : people) {
        person->print();
        //(*person).print();
        std::cout << "a" << std::endl;
    }

    //for (auto person : people) {
    //    delete person;
    //}

}
