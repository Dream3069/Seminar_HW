#include "Person.h"

Person::Person(const std::string& name) : name(name) {}

void Person::print() const {
    std::cout << "Name: " << name;
}
