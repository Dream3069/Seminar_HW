#include "Student.h"
#include "RecordBook.h"
#include <iostream>

Student::Student(const std::string& name) : Person(name) {
    recordBook = new RecordBook();
}

Student::Student(const std::string& name, const std::string& recordNum)
    : Person(name) {
    recordBook = new RecordBook(recordNum);
}

Student::~Student() {
    delete recordBook;
}

void Student::addGrade(int grade) {
    if (recordBook) {
        recordBook->addGrade(grade);
    }
}

double Student::getAverage() const {
    if (recordBook) {
        return recordBook->getAverage();
    }
    return 0.0;
}

void Student::print() const {
    Person::print();
    std::cout << ", ";
    if (recordBook) {
        recordBook->print();
    }
    else {
        std::cout << "no zachetka";
    }
}
