#include "Group.h"
#include "Student.h"
#include <algorithm>
#include <iostream>

Group::Group(const std::string& groupName) : name(groupName) {}

void Group::addStudent(Student* student) {
    if (student) {
        students.push_back(student);
    }
}

void Group::removeStudent(Student* student) {
    auto it = std::find(students.begin(), students.end(), student);
    if (it != students.end()) {
        students.erase(it);
    }
}

void Group::removeStudent(int index) {
    if (index >= 0 && index < static_cast<int>(students.size())) {
        students.erase(students.begin() + index);
    }
}

double Group::getGroupAverage() const {
    if (students.empty()) return 0.0;

    double totalSum = 0;
    for (const auto* student : students) {
        totalSum += student->getAverage();
    }
    return totalSum / students.size();
}

void Group::print() const {
    std::cout << "Group: " << name << "\n";
    std::cout << "Students (" << students.size() << "):\n";
    for (size_t i = 0; i < students.size(); ++i) {
        std::cout << "  " << i + 1 << ". ";
        students[i]->print();
        std::cout << "\n";
    }
    std::cout << "Average grade in group: " << getGroupAverage() << "\n";
}
