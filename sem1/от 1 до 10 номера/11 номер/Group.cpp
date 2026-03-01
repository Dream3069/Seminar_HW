#include "Group.hpp"
#include <iostream>
#include <algorithm>

Group::Group(const std::string& groupName) : name(groupName) {}

Group::~Group() {}

void Group::addStudent(Student* student) {
    if (student) {
        students.push_back(student);
    }
}

void Group::removeStudent(const std::string& studentName) {
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i] && students[i]->getName() == studentName) {
            students.erase(students.begin() + i);
            break;
        }
    }
}

void Group::printAll() const {
    std::cout << "Group: " << name << "\n";
    std::cout << "Students (" << students.size() << "):\n";
    for (size_t i = 0; i < students.size(); ++i) {
        std::cout << "  " << i + 1 << ". ";
        students[i]->print();
        std::cout << "\n";
    }
    std::cout << "Average grade in group: " << getAverageGrade() << "\n";
}

double Group::getAverageGrade() const {
    if (students.empty()) return 0.0;

    double sum = 0.0;
    for (const auto* student : students) {
        if (student) {
            sum += student->getAverage();
        }
    }
    return sum / students.size();
}

void Group::sortByName() {
    std::sort(students.begin(), students.end(),
        [](const Student* a, const Student* b) {
            if (!a || !b) return false;
            return a->getName() < b->getName();
        });
}

void Group::sortByAverage() {
    std::sort(students.begin(), students.end(),
        [](const Student* a, const Student* b) {
            if (!a || !b) return false;
            return a->getAverage() > b->getAverage();
        });
}

std::vector<Student*> Group::filterByAverage(double minAverage) const {
    std::vector<Student*> result;
    for (Student* student : students) {
        if (student && student->getAverage() >= minAverage) {
            result.push_back(student);
        }
    }
    return result;
}

bool Group::saveToFile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Eroor: do not open file\n";
        return false;
    }

    size_t nameLen = name.length();
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    file.write(name.c_str(), nameLen);

    size_t studentCount = students.size();
    file.write(reinterpret_cast<const char*>(&studentCount), sizeof(studentCount));

    for (const auto* student : students) {
        if (!student->save(file)) {
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

bool Group::loadFromFile(const std::string& filename) {
std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Eroor: do not open file for reading\n";
        return false;
    }
    
    size_t nameLen;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (!file.good()) {
        file.close();
        return false;
    }
    
    name.resize(nameLen);
    file.read(&name[0], nameLen);
    
    size_t studentCount;
    file.read(reinterpret_cast<char*>(&studentCount), sizeof(studentCount));
    if (!file.good()) {
        file.close();
        return false;
    }
    
    for (Student* s : students) {
        delete s;
    }
    students.clear();
    
    for (size_t i = 0; i < studentCount; ++i) {
        Student* student = new Student("");
        if (!student->load(file)) {
            delete student;
            for (Student* s : students) {
                delete s;
            }
            students.clear();
            file.close();
            return false;
        }
        students.push_back(student);
    }
    
    file.close();
    return true;
}
