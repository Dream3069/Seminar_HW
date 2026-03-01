#include "RecordBook.h"
#include <iostream>

RecordBook::RecordBook() : number("") {}

RecordBook::RecordBook(const std::string& num) : number(num) {}

void RecordBook::addGrade(int grade) {
    grades.push_back(grade);
}

double RecordBook::getAverage() const {
    if (grades.empty()) return 0.0;
    double sum = 0;
    for (int g : grades) {
        sum += g;
    }
    return sum / grades.size();
}

void RecordBook::print() const {
    std::cout << "zachetka " << number << ", grades: ";
    for (int g : grades) {
        std::cout << g << " ";
    }
    std::cout << "(Average. " << getAverage() << ")";
}
