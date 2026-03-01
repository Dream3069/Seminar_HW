#include "Teacher.h"
#include <iostream>

Teacher::Teacher(const std::string& name, const std::string& subj)
    : Person(name), subject(subj) {
}

void Teacher::print() const {
    Person::print();
    std::cout << ", Subject: " << subject;
}
