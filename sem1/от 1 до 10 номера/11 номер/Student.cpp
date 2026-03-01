#pragma once
#include "Student.hpp"
#include <iostream>

Student::Student(const std::string& name) : Person(name) {}

Student::Student(const std::string& name, const std::string& recordNum)
    : Person(name), recordBook(recordNum) {
}
const std::string& Student::getName() const {
    return name;
}
void Student::addGrade(int grade) {
    recordBook.addGrade(grade);
}


double Student::getAverage() const {
    return recordBook.getAverage();
}

void Student::print() const {
    Person::print();
    std::cout << ", ";
    recordBook.print();
}

bool Student::save(std::ofstream& out) const {
    if (!out.is_open()) return false;

    size_t len = name.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(name.c_str(), len);

    return recordBook.save(out);
}

bool Student::load(std::ifstream& in) {
    if (!in.is_open()) return false;

    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (!in.good()) return false;

    name.resize(len);
    in.read(&name[0], len);

    return recordBook.load(in);
}

